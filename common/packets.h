#ifndef PACKETS_H
#define PACKETS_H

#pragma pack(push, 1)

typedef enum packet_id_t {
    PKT_ID_S2C_CONFIRM = 1,
    PKT_ID_S2C_LOGIN_REQUEST = 2,
    PKT_ID_C2S_LOGIN_REQUEST = 3
} packet_id_t;

typedef struct packet_header_t {
    unsigned char version;
    packet_id_t id;
    int session_id;
    size_t payload_len;
} packet_header_t;

typedef struct s2c_confirm_payload_t {
    char ok;
    size_t msg_len;
    char *msg;
} s2c_confirm_payload_t;

typedef union payload_t {
    s2c_confirm_payload_t confirm;
    // others...
} payload_t;

typedef struct packet_t {
    packet_header_t header;
    payload_t payload;
} packet_t;

#pragma pack(pop)

size_t s2c_confirm(char *dst, size_t dst_size, int session_id, const s2c_confirm_payload_t *payload);
int des_s2c_confirm(const char *src, size_t src_len, s2c_confirm_payload_t *out_pyl);

#endif