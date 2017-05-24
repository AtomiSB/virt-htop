#include "tui_domain.h"

const char *tui_node_info_summary[TUI_NODE_INFO_SUMMARY_SIZE] = {
    "  domains:",
    "   active:",
    "  running:",
    "suspended:",
    "   paused:",
    " inactive:"
};

const char *tui_column_header[TUI_DOMAIN_COLUMN_SIZE] = {
    "ID  ",
    "DOMAIN",
    "STATE",
    "AUTOSTART",
    "MEM(%)",
    "REASON"
};

void tui_init_default_domain_columns(tui_domain_data *tui)
{
    for (int i = 0; i != TUI_DOMAIN_COLUMN_SIZE; ++i) {
        tui->domain_data[i] = NULL;
        tui->domain_data_item[i] = NULL;
        tui->domain_type[i] = i;
    }
    
    tui->domain_column       = NULL;

    tui->domain_columns_win  = NULL;
    tui->domain_columns_sub_win  = NULL;

    tui->domain_memory_size  = NULL;

    tui->domain_size = 0;
}

void tui_deinit_domain_columns(tui_domain_data *tui)
{
    /* free columns */
    if (tui->domain_column) {
        for (int i = 0; i != TUI_DOMAIN_COLUMN_SIZE; ++i) {
            if (tui->domain_column[i]) {
                unpost_menu(tui->domain_column[i]);
                free_menu(tui->domain_column[i]);
            }
        }
    }
    free(tui->domain_column);

    /* free items */
    for (int i = 0; i != TUI_DOMAIN_COLUMN_SIZE; ++i) {
        for (int j = 0; j != tui->domain_size; ++j)
            if (tui->domain_data_item[i][j])
                free_item(tui->domain_data_item[i][j]);
        free(tui->domain_data_item[i]);
    }

    /* free strings */
    for (int i = 0; i != TUI_DOMAIN_COLUMN_SIZE; ++i) {
        for (int j = 0; j != tui->domain_size; ++j)
            free(tui->domain_data[i][j]);
        free(tui->domain_data[i]);
    }
    if (tui->domain_columns_win) {
        for (int i = 0; i != TUI_DOMAIN_COLUMN_SIZE; ++i) 
            delwin(tui->domain_columns_sub_win[i]);
        free(tui->domain_columns_sub_win);
        delwin(tui->domain_columns_win);
    }
}

void tui_draw_column_header(tui_data *tui)
{
    int x   = 0;
    int y   = TUI_HEADER_HEIGHT-1;
    move(y, x);

    attron(COLOR_PAIR(TUI_COLOR_COLUMN_HEADER_TEXT));

    /* first print n empty spaces where n is the width of i column,
     * then print i column header at (y, x) and move over the y*/
    for (int i = 0; i != TUI_DOMAIN_COLUMN_SIZE; ++i) {
        for (int j = 0; j != tui_column_width[tui->domain_data->domain_type[i]]; ++j)
            printw(" ");
        mvprintw(y, x, tui_column_header[tui->domain_data->domain_type[i]]);
        x += tui_column_width[tui->domain_data->domain_type[i]];
    }

    /* fill up the rest of the screen */
    int max_x = 0;
    getmaxyx(stdscr, y, max_x);
    getyx(stdscr, y, x);
    for (int i = 0; i != max_x - x; ++i)
        printw(" ");
    attroff(COLOR_PAIR(TUI_COLOR_COLUMN_HEADER_TEXT));
}

ITEM **tui_create_items(char **begin, char **end)
{
    size_t dist = end - begin;

    ITEM **items = (ITEM **)calloc(dist, sizeof(ITEM *));

    for (int i = 0; i != dist; ++i)
        items[i] = new_item(begin[i], TUI_DOMAIN_COLUMN_SELECTOR);

    return items;
}

void tui_draw_domain_columns(tui_domain_data *tui)
{
    int x = 0, y = 0;
    /* length between previous and next printable column */
    size_t total_column_width = 0;
    getmaxyx(stdscr, x, y); y = 0;
    for (int i = 0; i != TUI_DOMAIN_COLUMN_SIZE; ++i) {
        set_menu_win(tui->domain_column[tui->domain_type[i]], tui->domain_columns_win);

        /* create subwindow that shows columns */
        tui->domain_columns_sub_win[tui->domain_type[i]] = 
            derwin( tui->domain_columns_win, 
                    x-TUI_HEADER_HEIGHT-1, 
                    tui_column_width[tui->domain_type[i]], 
                    y, 
                    total_column_width);

        /* update main window data before printing it out */
        touchwin(tui->domain_columns_win);
        set_menu_sub(   tui->domain_column[tui->domain_type[i]], 
                        tui->domain_columns_sub_win[tui->domain_type[i]]);
        /* set scrollable column up to x-TUI_HEADER_HEIGHT-1 elements in height 
           and 1 in width.*/
        set_menu_format(tui->domain_column[tui->domain_type[i]], x-TUI_HEADER_HEIGHT-1, 1);
        /* no column mark */
        set_menu_mark(tui->domain_column[tui->domain_type[i]], NULL);

        post_menu(tui->domain_column[tui->domain_type[i]]);

        total_column_width += tui_column_width[tui->domain_type[i]];
    }
}

void tui_create_columns(tui_domain_data *tui, virt_domain_data *data)
{
    /* last item counts as NULL */
    tui->domain_size = data->domain_size;

    /* create pointer to menus 'domain columns' */
    tui->domain_column = calloc(TUI_DOMAIN_COLUMN_SIZE, sizeof(MENU *));

    /* copy domain pointers */
    for (int i = 0; i != TUI_DOMAIN_COLUMN_SIZE; ++i) {
        tui->domain_data[i]         = data->domain_data[i];
        tui->domain_data_item[i]    = tui_create_items(data->domain_data[i], data->domain_data[i] + tui->domain_size);
    }

    /* create columns */
    tui->domain_column[TUI_DOMAIN_COLUMN_ID]            = 
        new_menu((ITEM **)tui->domain_data_item[tui->domain_type[VIRT_DOMAIN_DATA_TYPE_ID]]);
    tui->domain_column[TUI_DOMAIN_COLUMN_NAME]          = 
        new_menu((ITEM **)tui->domain_data_item[tui->domain_type[VIRT_DOMAIN_DATA_TYPE_NAME]]);
    tui->domain_column[TUI_DOMAIN_COLUMN_STATE]         =
        new_menu((ITEM **)tui->domain_data_item[tui->domain_type[VIRT_DOMAIN_DATA_TYPE_STATE]]);
    tui->domain_column[TUI_DOMAIN_COLUMN_AUTOSTART]     =
        new_menu((ITEM **)tui->domain_data_item[tui->domain_type[VIRT_DOMAIN_DATA_TYPE_AUTOSTART]]);
    tui->domain_column[TUI_DOMAIN_COLUMN_MEMORY_PRC]    =
        new_menu((ITEM **)tui->domain_data_item[tui->domain_type[VIRT_DOMAIN_DATA_TYPE_MEMORY_PRC]]);
    tui->domain_column[TUI_DOMAIN_COLUMN_REASON]        =
        new_menu((ITEM **)tui->domain_data_item[tui->domain_type[VIRT_DOMAIN_DATA_TYPE_REASON]]);

    /* set up default order */
    tui->domain_type[0] = TUI_DOMAIN_COLUMN_ID;
    tui->domain_type[1] = TUI_DOMAIN_COLUMN_NAME;
    tui->domain_type[2] = TUI_DOMAIN_COLUMN_STATE;
    tui->domain_type[3] = TUI_DOMAIN_COLUMN_AUTOSTART;
    tui->domain_type[4] = TUI_DOMAIN_COLUMN_MEMORY_PRC;
    tui->domain_type[5] = TUI_DOMAIN_COLUMN_REASON;

    /* copy memory size */
    tui->domain_memory_size = data->domain_memory_size;

    int x = 0, y = 0;
    getmaxyx(stdscr, x, y);

    /* create the window to be associated with the menu */
    tui->domain_columns_win = newwin(x-TUI_HEADER_HEIGHT-1, y, TUI_HEADER_HEIGHT, 0);
    tui->domain_columns_sub_win = malloc(sizeof(WINDOW*) * TUI_DOMAIN_COLUMN_SIZE);
    keypad(tui->domain_columns_win, TRUE);
}
