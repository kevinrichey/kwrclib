#include "kwrlib.h"
#include <stdlib.h>
#include <stdio.h>


void_fp swap_fp(void_fp *a, void_fp *b)
{
    void_fp t = *a;
    *a = *b;
    *b = t;
    return *a;
}

//----------------------------------------------------------------------
// Vital Signs Instrumentation Tools

const char *Vitals_CategoryName(Vitals_Category category)
{
#define X(cat, ___)  [Vitals_Category_##cat] = #cat,
    static const char *category_names[Vitals_Category_Size] = {
        VITALS_CATEGORY_TABLE 
    };
#undef X

    return category_names[category];
}

void Vitals_NullHandler(Vitals *unused, void *user_data) { }

void Vitals_UndefinedHandler(Vitals *vit, void *user_data) 
{
    fprintf(stderr, "OOPS: Undefined Vitals Category: %d\n", vit->category);
}

void Vitals_HandleFailure(Vitals *assertion, void *user_data)
{
    printf("%s: ASSERT: %s \"%s\" failed in function %s()\n", assertion->debug_info, Vitals_CategoryName(assertion->category), assertion->message, assertion->function);
    abort();
}

typedef struct Vitals_HandlerData {
    Vitals_HandlerFP  handler_fn;
    void             *user_data;
} Vitals_HandlerData;

#define X(___, handler)  { .handler_fn = handler },
static Vitals_HandlerData VITALS_HANDLERS[Vitals_Category_Size] = { 
    VITALS_CATEGORY_TABLE 
};
#undef X

Vitals_HandlerFP Vitals_SetHandler(Vitals_Category category, Vitals_HandlerFP new_handler)
{
    Vitals_HandlerFP old_handler = VITALS_HANDLERS[category].handler_fn;

    if (0 < category && category < Vitals_Category_End) {
        VITALS_HANDLERS[category] = (Vitals_HandlerData){ .handler_fn = new_handler };
    }

    return old_handler;
}

Vitals_HandlerFP Vitals_SetHandlerData(Vitals_Category category, Vitals_HandlerFP new_handler, void *user_data)
{
    Vitals_HandlerFP old_handler = VITALS_HANDLERS[category].handler_fn;

    if (0 < category && category < Vitals_Category_End) {
        VITALS_HANDLERS[category] = (Vitals_HandlerData){ .handler_fn = new_handler, .user_data = user_data };
    }

    return old_handler;
}

Vitals_HandlerFP Vitals_GetHandler(Vitals_Category category)
{
    return (0 < category && category < Vitals_Category_End)?  VITALS_HANDLERS[category].handler_fn:  Vitals_UndefinedHandler;
}

void CheckVitals(Vitals *vits)
{
    if (0 < vits->category && vits->category < Vitals_Category_End) {
        VITALS_HANDLERS[vits->category].handler_fn(vits, VITALS_HANDLERS[vits->category].user_data);
    }
}

