#include <libpull/memory/simple_manifest.h>
#include <libpull/memory/manifest.h>
#include <libpull/common.h>
#include <string.h>

#ifdef SIMPLE_MANIFEST

version_t get_version_impl(const manifest_t* mt) {
    return mt->vendor.version;
}
address_t get_size_impl(const manifest_t* mt) {
    return mt->vendor.size;
}
address_t get_offset_impl(const manifest_t* mt) {
    return mt->vendor.offset;
}
uint8_t* get_digest_impl(const manifest_t* mt) {
    return (uint8_t*) mt->vendor.digest;
}
uint8_t* get_server_key_x_impl(const manifest_t* mt) {
    return (uint8_t*) mt->vendor.server_key_x;
}
uint8_t* get_server_key_y_impl(const manifest_t* mt) {
    return (uint8_t*) mt->vendor.server_key_y;
}

appid_t get_appid_impl(const manifest_t* mt) {
    return mt->vendor.appid;
}
version_t get_diff_version_impl(const manifest_t* mt) {
    return mt->server.diff_version;
}
address_t get_prop_size_impl(const manifest_t* mt) {
    return mt->server.prop_size;
}
nonce_t get_nonce_impl(const manifest_t* mt) {
    return mt->server.nonce;
}
udid_t get_udid_impl(const manifest_t* mt) {
    return mt->server.udid;
}
void set_diff_version_impl(manifest_t* mt, version_t diff_version) {
    mt->server.diff_version = diff_version;
}
void set_nonce_impl(manifest_t* mt, nonce_t nonce) {
    mt->server.nonce = nonce;
}
void set_udid_impl(manifest_t* mt, udid_t udid) {
    mt->server.udid = udid;
}
void set_appid_impl(manifest_t* mt, appid_t appid) {
    mt->vendor.appid = appid;
}
void set_version_impl(manifest_t* mt, version_t version) {
    mt->vendor.version = version;
}
void set_size_impl(manifest_t* mt, address_t size) {
    mt->vendor.size = size;
}
void set_prop_size_impl(manifest_t* mt, address_t size) {
    mt->server.prop_size = size;
}
void set_offset_impl(manifest_t* mt, address_t offset) {
    mt->vendor.offset = offset;
}
void set_digest_impl(manifest_t* mt, uint8_t* digest) {
    memcpy(mt->vendor.digest, digest, 32); // XXX hardcoded value
}
void set_server_key_x_impl(manifest_t* mt, uint8_t* server_key_x) {
    memcpy(mt->vendor.server_key_x, server_key_x, 32); // XXX hardcoded value
}
void set_server_key_y_impl(manifest_t* mt, uint8_t* server_key_y) {
    memcpy(mt->vendor.server_key_y, server_key_y, 32); // XXX hardcoded value
}
/* Memory getter and setters */
uint8_t* get_vendor_signature_r_impl(const manifest_t *mt, uint8_t* size) {
    *size = 32;
    return (uint8_t*) (mt->vendor_signature_r);
}
uint8_t* get_vendor_signature_s_impl(const manifest_t *mt, uint8_t* size) {
    *size = 32;
    return (uint8_t*) (mt->vendor_signature_s);
}
uint8_t* get_server_signature_r_impl(const manifest_t *mt, uint8_t* size) {
    *size = 32;
    return (uint8_t*) mt->server_signature_r;
}
uint8_t* get_server_signature_s_impl(const manifest_t *mt, uint8_t* size) {
    *size = 32;
    return (uint8_t*) mt->server_signature_s;
}


int set_vendor_signature_r_impl(manifest_t *mt, uint8_t* vendor_signature_r, uint8_t size) {
    if (size != 32) { // XXX hardcoded value
        return 1;
    }
    return memcpy(mt->vendor_signature_r, vendor_signature_r, size) != mt->vendor_signature_r;
}
int set_vendor_signature_s_impl(manifest_t *mt, uint8_t* vendor_signature_s, uint8_t size) {
    if (size != 32) { // XXX hardcoded value
        return 1;
    }
    return memcpy(mt->vendor_signature_s, vendor_signature_s, size) != mt->vendor_signature_s;
}
int set_server_signature_r_impl(manifest_t *mt, uint8_t* server_signature_r, uint8_t size) {
    if (size != 32) { // XXX hardcoded value
        return 1;
    }
    return memcpy(mt->server_signature_r, server_signature_r, size) != mt->server_signature_r;
}
int set_server_signature_s_impl(manifest_t *mt, uint8_t* server_signature_s, uint8_t size) {
    if (size != 32) { // XXX hardcoded value
        return 1;
    }
    return memcpy(mt->server_signature_s, server_signature_s, size) != mt->server_signature_s;
}

pull_error verify_signature_impl(manifest_t* mt, keystore_t keystore) {
    digest_ctx ctx;
    if (digest_init(&ctx) != PULL_SUCCESS) {
        return GENERIC_ERROR; // TODO specialize error
    }
    digest_update(&ctx, &mt->vendor, sizeof(vendor_manifest_t));
    uint8_t* hash = (uint8_t*) digest_finalize(&ctx);
    if (ecc_verify(keystore.x, keystore.y, mt->vendor_signature_r, mt->vendor_signature_s, hash, get_digest_size()) != PULL_SUCCESS) {
        return GENERIC_ERROR;
    }
    if (digest_init(&ctx) != PULL_SUCCESS) {
        return GENERIC_ERROR; // TODO specialize error
    }
    digest_update(&ctx, &mt->server, sizeof(server_manifest_t));
    hash = (uint8_t*) digest_finalize(&ctx);
    if (ecc_verify(mt->vendor.server_key_x, mt->vendor.server_key_y, mt->server_signature_r, mt->server_signature_s, hash, get_digest_size()) != PULL_SUCCESS) {
        return GENERIC_ERROR;
    }
    return PULL_SUCCESS;
}

#ifdef ENABLE_SIGN
static pull_error sign_data_impl(uint8_t* data, size_t size, const uint8_t *private_key,
                                    uint8_t* signature_buffer) {
    digest_ctx ctx;
    pull_error err = digest_init(&ctx);
    if (err) {
        return GENERIC_ERROR; // TODO specialize error
    }
    digest_update(&ctx, data, size);
    uint8_t* hash = (uint8_t*) digest_finalize(&ctx);
    if (ecc_sign(private_key, signature_buffer, hash, get_digest_size()) != PULL_SUCCESS) {
        return GENERIC_ERROR;
    }
    return PULL_SUCCESS;
}

pull_error sign_manifest_vendor_impl(manifest_t* mt, const uint8_t *private_key,
                                   uint8_t* signature_buffer) {
    pull_error err = sign_data_impl((uint8_t*) &mt->vendor, sizeof(vendor_manifest_t), private_key, signature_buffer);
    set_vendor_signature_r_impl(mt, signature_buffer, get_curve_size());
    set_vendor_signature_s_impl(mt, signature_buffer+get_curve_size(), get_curve_size());
    return err;
}

pull_error sign_manifest_server_impl(manifest_t* mt, const uint8_t *private_key,
                                   uint8_t* signature_buffer) {
    pull_error err = sign_data_impl((uint8_t*) &mt->server, sizeof(server_manifest_t), private_key, signature_buffer);
    set_server_signature_r_impl(mt, signature_buffer, get_curve_size());
    set_server_signature_s_impl(mt, signature_buffer+get_curve_size(), get_curve_size());
    return err;
}
#endif /* ENABLE_SIGN  */

void print_manifest_impl(const manifest_t* mt) {
    log_info("Version: %04x\n", mt->vendor.version);
    log_info("Size: %d\n", (int) mt->vendor.size);
}

#endif /* SIMPLE_MANIFEST */
