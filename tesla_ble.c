#include "hid_tesla.h"
#include "hid.h"
#include <gui/elements.h>

#define TAG "HidTesla"

struct HidTesla {
    View* view;
    Hid* hid;
};

typedef struct {
    bool left_pressed;
    bool up_pressed;
    bool right_pressed;
    bool down_pressed;
    bool ok_pressed;
    bool connected;
    bool is_cursor_set;
    bool back_mouse_pressed;
} HidTeslaModel;

static void hid_tesla_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);

    // Header
#ifdef HID_TRANSPORT_BLE
    if(model->connected) {
        canvas_draw_icon(canvas, 0, 0, &I_Ble_connected_15x15);
    } else {
        canvas_draw_icon(canvas, 0, 0, &I_Ble_disconnected_15x15);
    }
#endif

    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(canvas, 17, 3, AlignLeft, AlignTop, "Tesla");
    canvas_set_font(canvas, FontSecondary);

    elements_multiline_text_aligned(canvas, 13, 62, AlignLeft, AlignBottom, "Hold to exit");
}

static void hid_tesla_reset_cursor(HidTesla* hid_tesla) {
    // Set cursor to the phone's left up corner
    // Delays to guarantee one packet per connection interval
    for(size_t i = 0; i < 8; i++) {
        hid_hal_mouse_move(hid_tesla->hid, -127, -127);
        furi_delay_ms(50);
    }
    // Move cursor from the corner
    hid_hal_mouse_move(hid_tesla->hid, 20, 120);
    furi_delay_ms(50);
}
static void send_text(HidTesla* hid_tesla, const char* text) {
    while(*text) {
        hid_hal_keyboard_press(hid_tesla->hid, *text);
        furi_delay_ms(25);
        hid_hal_keyboard_release(hid_tesla->hid, *text);
        furi_delay_ms(25);
        text++;
    }
}

static void hid_tesla_process_press(HidTesla* hid_tesla, HidTeslaModel* model, InputEvent* event) {
    if(event->key == InputKeyUp) {
        model->up_pressed = true;
    } else if(event->key == InputKeyDown) {
        model->down_pressed = true;
    } else if(event->key == InputKeyLeft) {
        model->left_pressed = true;
        hid_hal_consumer_key_press(hid_tesla->hid, HID_CONSUMER_VOLUME_DECREMENT);
    } else if(event->key == InputKeyRight) {
        model->right_pressed = true;
        hid_hal_consumer_key_press(hid_tesla->hid, HID_CONSUMER_VOLUME_INCREMENT);
    } else if(event->key == InputKeyOk) {
        model->ok_pressed = true;

        // Send email
        send_text(hid_tesla, "example@gmail.com");
        hid_hal_keyboard_press(hid_tesla->hid, HID_KEYBOARD_RETURN);

        furi_delay_ms(1000); // wait 1 second

        // Send password
        send_text(hid_tesla, "Password!");
        hid_hal_keyboard_press(hid_tesla->hid, HID_KEYBOARD_RETURN);
        furi_delay_ms(1000); // wait 1 second

        // Send OTP
        send_text(hid_tesla, "123456");
        hid_hal_keyboard_press(hid_tesla->hid, HID_KEYBOARD_RETURN);
        furi_delay_ms(25);
    } else if(event->key == InputKeyBack) {
        model->back_mouse_pressed = true;
    }
}

static void
    hid_tesla_process_release(HidTesla* hid_tesla, HidTeslaModel* model, InputEvent* event) {
    if(event->key == InputKeyUp) {
        model->up_pressed = false;
    } else if(event->key == InputKeyDown) {
        model->down_pressed = false;
    } else if(event->key == InputKeyLeft) {
        model->left_pressed = false;
        hid_hal_consumer_key_release(hid_tesla->hid, HID_CONSUMER_VOLUME_DECREMENT);
    } else if(event->key == InputKeyRight) {
        model->right_pressed = false;
        hid_hal_consumer_key_release(hid_tesla->hid, HID_CONSUMER_VOLUME_INCREMENT);
    } else if(event->key == InputKeyOk) {
        model->ok_pressed = false;
    } else if(event->key == InputKeyBack) {
        model->back_mouse_pressed = false;
    }
}

static bool hid_tesla_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    HidTesla* hid_tesla = context;
    bool consumed = false;

    with_view_model(
        hid_tesla->view,
        HidTeslaModel * model,
        {
            if(event->type == InputTypePress) {
                hid_tesla_process_press(hid_tesla, model, event);
                if(model->connected && !model->is_cursor_set) {
                    hid_tesla_reset_cursor(hid_tesla);
                    model->is_cursor_set = true;
                }
                consumed = true;
            } else if(event->type == InputTypeRelease) {
                hid_tesla_process_release(hid_tesla, model, event);
                consumed = true;
            } else if(event->type == InputTypeShort) {
                if(event->key == InputKeyOk) {
                    hid_hal_mouse_press(hid_tesla->hid, HID_MOUSE_BTN_LEFT);
                    furi_delay_ms(25);
                    hid_hal_mouse_release(hid_tesla->hid, HID_MOUSE_BTN_LEFT);
                    furi_delay_ms(100);
                    hid_hal_mouse_press(hid_tesla->hid, HID_MOUSE_BTN_LEFT);
                    furi_delay_ms(25);
                    hid_hal_mouse_release(hid_tesla->hid, HID_MOUSE_BTN_LEFT);
                    consumed = true;
                } else if(event->key == InputKeyUp) {
                    // Swipe to previous video
                    hid_hal_mouse_scroll(hid_tesla->hid, -6);
                    hid_hal_mouse_scroll(hid_tesla->hid, -8);
                    hid_hal_mouse_scroll(hid_tesla->hid, -10);
                    hid_hal_mouse_scroll(hid_tesla->hid, -8);
                    hid_hal_mouse_scroll(hid_tesla->hid, -6);
                    consumed = true;
                } else if(event->key == InputKeyDown) {
                    // Swipe to next video
                    hid_hal_mouse_scroll(hid_tesla->hid, 6);
                    hid_hal_mouse_scroll(hid_tesla->hid, 8);
                    hid_hal_mouse_scroll(hid_tesla->hid, 10);
                    hid_hal_mouse_scroll(hid_tesla->hid, 8);
                    hid_hal_mouse_scroll(hid_tesla->hid, 6);
                    consumed = true;
                } else if(event->key == InputKeyBack) {
                    // Pause
                    hid_hal_mouse_press(hid_tesla->hid, HID_MOUSE_BTN_LEFT);
                    furi_delay_ms(50);
                    hid_hal_mouse_release(hid_tesla->hid, HID_MOUSE_BTN_LEFT);
                    consumed = true;
                }
            } else if(event->type == InputTypeLong) {
                if(event->key == InputKeyBack) {
                    hid_hal_consumer_key_release_all(hid_tesla->hid);
                    model->is_cursor_set = false;
                    consumed = false;
                }
            }
        },
        true);

    return consumed;
}

HidTesla* hid_tesla_alloc(Hid* bt_hid) {
    HidTesla* hid_tesla = malloc(sizeof(HidTesla));
    hid_tesla->hid = bt_hid;
    hid_tesla->view = view_alloc();
    view_set_context(hid_tesla->view, hid_tesla);
    view_allocate_model(hid_tesla->view, ViewModelTypeLocking, sizeof(HidTeslaModel));
    view_set_draw_callback(hid_tesla->view, hid_tesla_draw_callback);
    view_set_input_callback(hid_tesla->view, hid_tesla_input_callback);

    return hid_tesla;
}

void hid_tesla_free(HidTesla* hid_tesla) {
    furi_assert(hid_tesla);
    view_free(hid_tesla->view);
    free(hid_tesla);
}

View* hid_tesla_get_view(HidTesla* hid_tesla) {
    furi_assert(hid_tesla);
    return hid_tesla->view;
}

void hid_tesla_set_connected_status(HidTesla* hid_tesla, bool connected) {
    furi_assert(hid_tesla);
    with_view_model(
        hid_tesla->view,
        HidTeslaModel * model,
        {
            model->connected = connected;
            model->is_cursor_set = false;
        },
        true);
}
