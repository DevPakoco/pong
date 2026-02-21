#include <stdlib.h>
#include <string.h>
#include "packets.h"

size_t serialize_header(char *dst, size_t dst_size, int session_id, packet_id_t packet_id, size_t payload_len) {
    const size_t header_size = sizeof(packet_header_t);
    if (dst_size < header_size) return 0;
    const packet_header_t hdr = {
        .version     = 1,
        .id          = packet_id,
        .session_id  = session_id,
        .payload_len = payload_len
    };
    memcpy(dst, &hdr, header_size);
    return header_size;
}

int deserialize_header(const char *src, size_t src_len, packet_header_t *out_hdr) {
    const size_t header_size = sizeof(packet_header_t);
    if (!src || !out_hdr || src_len < header_size) return 1;
    memcpy(out_hdr, src, header_size);
    return 0;
}

size_t serialize(char *buff, size_t buff_size, int session_id, packet_id_t id, const void *payload, size_t payload_len) {
    const size_t total_len = sizeof(packet_header_t) + payload_len;
    if (buff_size < total_len) return 0;
    size_t pos = serialize_header(buff, buff_size, session_id, id, payload_len);
    if (pos == 0) return 0;
    if (payload_len > 0) memcpy(buff + pos, payload, payload_len);
    return total_len;
}

size_t s2c_confirm(char *dst, size_t dst_size, int session_id, const s2c_confirm_payload_t *payload) {
    if (payload->msg == NULL && payload->msg_len != 0) return 0;
    const size_t payload_len = 1 + sizeof(size_t) + payload->msg_len;
    const size_t total_len   = sizeof(packet_header_t) + payload_len;
    if (dst_size < total_len) return 0;
    size_t pos = serialize_header(dst, dst_size, session_id, PKT_ID_S2C_CONFIRM, payload_len);
    if (pos == 0) return 0;
    char *p = dst + pos;
    *p++ = payload->ok;
    memcpy(p, &payload->msg_len, sizeof(size_t));
    p += sizeof(size_t);
    if (payload->msg_len > 0) memcpy(p, payload->msg, payload->msg_len);
    return total_len;
}

int des_s2c_confirm(const char *src, size_t src_len, s2c_confirm_payload_t *out_pyl) {
    if (!src || !out_pyl || src_len < sizeof(packet_header_t)) return 1;
    const char *p = src + sizeof(packet_header_t);
    const char *end = src + src_len;
    if (end - p < 1 + sizeof(size_t)) return 1;
    out_pyl->ok = *p++;
    memcpy(&out_pyl->msg_len, p, sizeof(size_t));
    p += sizeof(size_t);
    if (end - p < out_pyl->msg_len) return 1;
    if (out_pyl->msg_len == 0) {
        out_pyl->msg = NULL;
        return 0;
    }
    char *msg = malloc(out_pyl->msg_len);
    if (!msg) return 1;
    memcpy(msg, p, out_pyl->msg_len);
    msg[out_pyl->msg_len] = '\0';
    out_pyl->msg = msg;
    return 0;
}