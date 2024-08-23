#pragma once

#include <gui/view.h>

typedef struct Hid Hid;
typedef struct HidTesla HidTesla;

HidTesla* hid_tesla_alloc(Hid* bt_hid);

void hid_tesla_free(HidTesla* hid_tesla);

View* hid_tesla_get_view(HidTesla* hid_tesla);

void hid_tesla_set_connected_status(HidTesla* hid_tesla, bool connected);
