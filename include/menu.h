#ifndef MENU_H
#define MENU_H

#define MAX_ITEMS       128
#define MAX_LABEL       128
#define MAX_DESC        512
#define MAX_CATEGORIES  16

typedef enum {
    ACTION_SCRIPT,
    ACTION_DOTFILES,
    ACTION_DOTFILES_FILE,  /* single file pulled via curl  */
    ACTION_HW_SELECT       /* hardware-aware install flow  */
} ActionType;

/* Identifies which hardware-aware script to run */
typedef enum {
    HW_FLOW_OWLSLY_DESKTOP,
    HW_FLOW_GAMING
} HwFlow;

typedef struct MenuItem {
    char          label[MAX_LABEL];
    char          desc[MAX_DESC];
    ActionType    type;
    const char   *script;          /* shell cmd or script path        */
    const char   *dotfiles_app;    /* folder name under .config/      */
    const char   *dotfiles_url;    /* raw URL for single-file pulls   */
    const char   *dotfiles_dest;   /* destination path for file pulls */
    HwFlow        hw_flow;          /* which flow to run (ACTION_HW_SELECT) */
} MenuItem;

typedef struct Category {
    char      label[MAX_LABEL];
    MenuItem  items[MAX_ITEMS];
    int       item_count;
} Category;

typedef struct MenuState {
    Category  categories[MAX_CATEGORIES];
    int       cat_count;
    int       selected_cat;
    int       selected_item;
    int       scroll_offset;
} MenuState;

void menu_init(MenuState *state);

#endif /* MENU_H */
