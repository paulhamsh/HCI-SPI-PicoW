#include "py/runtime.h"
#include "py/objstr.h"
#include "py/obj.h"

#include "lib/cyw43-driver/src/cyw43.h"

#define CYW43_ENABLE_BLUETOOTH 1


#define BUF_MAX 500
uint8_t buf[BUF_MAX];
uint32_t buf_len;


extern int cyw43_spi_transfer(void *self, const uint8_t *tx, size_t tx_length, uint8_t *rx, size_t rx_length);


// This structure represents HCI instance objects.
typedef struct _HCI_SPI_obj_t {
    // All objects start with the base.
    mp_obj_base_t base;

    // Everything below can be thought of as instance attributes, but they
    // cannot be accessed by MicroPython code directly.
    mp_uint_t initialised;
} HCI_SPI_obj_t;

#define PRE_BUFFER_LEN 3

STATIC mp_obj_t HCI_SPI_receive_raw(mp_obj_t self_in ) {
    int ret;
    uint32_t len;

    // if buffer is empty do next read, otherwise use data from last readable()

    if (buf_len == 0) {
        ret = cyw43_bluetooth_hci_read(buf, BUF_MAX, &len);
        if (ret) len = 0;
    }
    else
        len = buf_len;

    // Remove the padding
    if (len > PRE_BUFFER_LEN) len -= PRE_BUFFER_LEN;

    buf_len = 0;
    return mp_obj_new_bytes(&buf[PRE_BUFFER_LEN], len);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(HCI_SPI_receive_raw_obj, HCI_SPI_receive_raw);




STATIC mp_obj_t HCI_SPI_send_raw(mp_obj_t self_in, mp_obj_t data_obj ) {
    mp_check_self(mp_obj_is_str_or_bytes(data_obj));
    GET_STR_DATA_LEN(data_obj, data, data_len);

    memcpy(&buf[PRE_BUFFER_LEN], data, data_len);
    //buf[PRE_BUFFER_LEN - 1] = buf[PRE_BUFFER_LEN];
    cyw43_bluetooth_hci_write(buf, data_len + PRE_BUFFER_LEN);

    return mp_obj_new_bool(1);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_2(HCI_SPI_send_raw_obj, HCI_SPI_send_raw);




extern  bool hci_transport_ready;

STATIC mp_obj_t HCI_SPI_readable(mp_obj_t self_in) {
    // The first argument is self. It is cast to the *HCI_obj_t
    // type so we can read its attributes.
    // HCI_obj_t *self = MP_OBJ_TO_PTR(self_in);

    int ret;
    uint32_t len;

    ret = cyw43_bluetooth_hci_read(buf, BUF_MAX, &len);

    if (ret) len = 0;
    buf_len = len;

    return mp_obj_new_bool(buf_len > 0);
//    return mp_obj_new_bool(hci_transport_ready);

}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(HCI_SPI_readable_obj, HCI_SPI_readable);




STATIC mp_obj_t HCI_SPI_transfer(size_t n_args, const mp_obj_t *args) {
    mp_check_self(mp_obj_is_str_or_bytes(args[1]));
    GET_STR_DATA_LEN(args[1], data, data_len);

    int send_len    = mp_obj_get_int(args[2]);
    int receive_len = mp_obj_get_int(args[3]);

    memcpy(buf, data, data_len);

    cyw43_spi_transfer(&cyw43_state, data, send_len, buf, receive_len);

    return mp_obj_new_bytes(buf, receive_len);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(HCI_SPI_transfer_obj, 4, 4, HCI_SPI_transfer);



extern bool hci_transport_ready;

STATIC mp_obj_t HCI_SPI_transport_ready(mp_obj_t self_in) {
    return mp_obj_new_bool(hci_transport_ready);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(HCI_SPI_transport_ready_obj, HCI_SPI_transport_ready);



// This represents HCI_SPI.__new__ and HCI_SPI.__init__, which is called when
// the user instantiates a HCI_SPI object.

STATIC mp_obj_t HCI_SPI_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {

    // Allocates the new object and sets the type.
    HCI_SPI_obj_t *self = mp_obj_malloc(HCI_SPI_obj_t, type);

    cyw43_init(&cyw43_state);
    cyw43_bluetooth_hci_init();  // load bt firmware now rather than later
    buf_len = 0;

    self->initialised = 1;

    return MP_OBJ_FROM_PTR(self);
}

// This collects all methods and other static class attributes of the HCI.

STATIC const mp_rom_map_elem_t HCI_SPI_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_readable), MP_ROM_PTR(&HCI_SPI_readable_obj) },
    { MP_ROM_QSTR(MP_QSTR_send_raw), MP_ROM_PTR(&HCI_SPI_send_raw_obj) },
    { MP_ROM_QSTR(MP_QSTR_receive_raw), MP_ROM_PTR(&HCI_SPI_receive_raw_obj) },
    { MP_ROM_QSTR(MP_QSTR_transfer), MP_ROM_PTR(&HCI_SPI_transfer_obj) },
    { MP_ROM_QSTR(MP_QSTR_transport_ready), MP_ROM_PTR(&HCI_SPI_transport_ready_obj) },
};

STATIC MP_DEFINE_CONST_DICT(HCI_SPI_locals_dict, HCI_SPI_locals_dict_table);

// This defines the type(HCI) object.
MP_DEFINE_CONST_OBJ_TYPE(
    type_HCI_SPI,
    MP_QSTR_HCI_SPI,
    MP_TYPE_FLAG_NONE,
    make_new, HCI_SPI_make_new,
    locals_dict, &HCI_SPI_locals_dict
    );

// Define all attributes of the module.
// Table entries are key/value pairs of the attribute name (a string)
// and the MicroPython object reference.

STATIC const mp_rom_map_elem_t module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_HCI_SPI_PicoW) },
    { MP_ROM_QSTR(MP_QSTR_HCI_SPI),    MP_ROM_PTR(&type_HCI_SPI) },
};

STATIC MP_DEFINE_CONST_DICT(module_globals, module_globals_table);

// Define module object.

const mp_obj_module_t example_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&module_globals,
};

// Register the module to make it available in Python.

MP_REGISTER_MODULE(MP_QSTR_HCI_SPI_PicoW, example_user_cmodule);
