/*
 * This is the source code of tgnet library v. 1.1
 * It is licensed under GNU GPL v. 2 or later.
 * You should have received a copy of the license in this archive (see LICENSE).
 *
 * Copyright Nikolai Kudashov, 2015-2018.
 */

#include "ApiScheme.h"
#include "ByteArray.h"
#include "NativeByteBuffer.h"
#include "FileLog.h"

Bool *Bool::TLdeserialize(NativeByteBuffer *stream, uint32_t constructor, int32_t instanceNum, bool &error) {
    Bool *result = nullptr;
    switch (constructor) {
        case 0x997275b5:
            result = new TL_boolTrue();
            break;
        case 0xbc799737:
            result = new TL_boolFalse();
            break;
        default:
            error = true;
            if (LOGS_ENABLED) DEBUG_E("can't parse magic %x in Bool", constructor);
            return nullptr;
    }
    result->readParams(stream, instanceNum, error);
    return result;
}

void TL_boolTrue::serializeToStream(NativeByteBuffer *stream) {
    stream->writeInt32(constructor);
}

void TL_boolFalse::serializeToStream(NativeByteBuffer *stream) {
    stream->writeInt32(constructor);
}

TL_dcOption *TL_dcOption::TLdeserialize(NativeByteBuffer *stream, uint32_t constructor, int32_t instanceNum, bool &error) {
    if (TL_dcOption::constructor != constructor) {
        error = true;
        if (LOGS_ENABLED) DEBUG_E("can't parse magic %x in TL_dcOption", constructor);
        return nullptr;
    }
    TL_dcOption *result = new TL_dcOption();
    result->readParams(stream, instanceNum, error);
    return result;
}

void TL_dcOption::readParams(NativeByteBuffer *stream, int32_t instanceNum, bool &error) {
    flags = stream->readInt32(&error);
    ipv6 = (flags & 1) != 0;
    media_only = (flags & 2) != 0;
    tcpo_only = (flags & 4) != 0;
    cdn = (flags & 8) != 0;
    isStatic = (flags & 16) != 0;
    id = stream->readInt32(&error);
    ip_address = stream->readString(&error);
    port = stream->readInt32(&error);
    if ((flags & 1024) != 0) {
        secret = std::unique_ptr<ByteArray>(stream->readByteArray(&error));
    }
}

void TL_dcOption::serializeToStream(NativeByteBuffer *stream) {
    stream->writeInt32(constructor);
    flags = ipv6 ? (flags | 1) : (flags &~ 1);
    flags = media_only ? (flags | 2) : (flags &~ 2);
    flags = tcpo_only ? (flags | 4) : (flags &~ 4);
    flags = cdn ? (flags | 8) : (flags &~ 8);
    flags = isStatic ? (flags | 16) : (flags &~ 16);
    stream->writeInt32(flags);
    stream->writeInt32(id);
    stream->writeString(ip_address);
    stream->writeInt32(port);
    if ((flags & 1024) != 0) {
        stream->writeByteArray(secret.get());
    }
}

TL_cdnPublicKey *TL_cdnPublicKey::TLdeserialize(NativeByteBuffer *stream, uint32_t constructor, int32_t instanceNum, bool &error) {
    if (TL_cdnPublicKey::constructor != constructor) {
        error = true;
        if (LOGS_ENABLED) DEBUG_E("can't parse magic %x in TL_cdnPublicKey", constructor);
        return nullptr;
    }
    TL_cdnPublicKey *result = new TL_cdnPublicKey();
    result->readParams(stream, instanceNum, error);
    return result;
}

void TL_cdnPublicKey::readParams(NativeByteBuffer *stream, int32_t instanceNum, bool &error) {
    dc_id = stream->readInt32(&error);
    public_key = stream->readString(&error);
}

void TL_cdnPublicKey::serializeToStream(NativeByteBuffer *stream) {
    stream->writeInt32(constructor);
    stream->writeInt32(dc_id);
    stream->writeString(public_key);
}

TL_cdnConfig *TL_cdnConfig::TLdeserialize(NativeByteBuffer *stream, uint32_t constructor, int32_t instanceNum, bool &error) {
    if (TL_cdnConfig::constructor != constructor) {
        error = true;
        if (LOGS_ENABLED) DEBUG_E("can't parse magic %x in TL_cdnConfig", constructor);
        return nullptr;
    }
    TL_cdnConfig *result = new TL_cdnConfig();
    result->readParams(stream, instanceNum, error);
    return result;
}

void TL_cdnConfig::readParams(NativeByteBuffer *stream, int32_t instanceNum, bool &error) {
    int magic = stream->readInt32(&error);
    if (magic != 0x1cb5c415) {
        error = true;
        if (LOGS_ENABLED) DEBUG_E("wrong Vector magic, got %x", magic);
        return;
    }
    int count = stream->readInt32(&error);
    for (int32_t a = 0; a < count; a++) {
        TL_cdnPublicKey *object = TL_cdnPublicKey::TLdeserialize(stream, stream->readUint32(&error), instanceNum, error);
        if (object == nullptr) {
            return;
        }
        public_keys.push_back(std::unique_ptr<TL_cdnPublicKey>(object));
    }
}

void TL_cdnConfig::serializeToStream(NativeByteBuffer *stream) {
    stream->writeInt32(constructor);
    stream->writeInt32(0x1cb5c415);
    int32_t count = (int32_t) public_keys.size();
    stream->writeInt32(count);
    for (int32_t a = 0; a < count; a++) {
        public_keys[a]->serializeToStream(stream);
    }
}

bool TL_help_getCdnConfig::isNeedLayer() {
    return true;
}

TLObject *TL_help_getCdnConfig::deserializeResponse(NativeByteBuffer *stream, uint32_t constructor, int32_t instanceNum, bool &error) {
    return TL_cdnConfig::TLdeserialize(stream, constructor, instanceNum, error);
}

void TL_help_getCdnConfig::serializeToStream(NativeByteBuffer *stream) {
    stream->writeInt32(constructor);
}

TL_config *TL_config::TLdeserialize(NativeByteBuffer *stream, uint32_t constructor, int32_t instanceNum, bool &error) {
    if (TL_config::constructor != constructor) {
        error = true;
        if (LOGS_ENABLED) DEBUG_E("can't parse magic %x in TL_config", constructor);
        return nullptr;
    }
    TL_config *result = new TL_config();
    result->readParams(stream, instanceNum, error);
    return result;
}

void TL_config::readParams(NativeByteBuffer *stream, int32_t instanceNum, bool &error) {
    flags = stream->readInt32(&error);
    date = stream->readInt32(&error);
    expires = stream->readInt32(&error);
    test_mode = stream->readBool(&error);
    this_dc = stream->readInt32(&error);
    uint32_t magic = stream->readUint32(&error);
    if (magic != 0x1cb5c415) {
        error = true;
        if (LOGS_ENABLED) DEBUG_E("wrong Vector magic, got %x", magic);
        return;
    }
    int32_t count = stream->readInt32(&error);
    for (int32_t a = 0; a < count; a++) {
        TL_dcOption *object = TL_dcOption::TLdeserialize(stream, stream->readUint32(&error), instanceNum, error);
        if (object == nullptr) {
            return;
        }
        dc_options.push_back(std::unique_ptr<TL_dcOption>(object));
    }
    dc_txt_domain_name = stream->readString(&error);
    chat_size_max = stream->readInt32(&error);
    megagroup_size_max = stream->readInt32(&error);
    forwarded_count_max = stream->readInt32(&error);
    online_update_period_ms = stream->readInt32(&error);
    offline_blur_timeout_ms = stream->readInt32(&error);
    offline_idle_timeout_ms = stream->readInt32(&error);
    online_cloud_timeout_ms = stream->readInt32(&error);
    notify_cloud_delay_ms = stream->readInt32(&error);
    notify_default_delay_ms = stream->readInt32(&error);
    push_chat_period_ms = stream->readInt32(&error);
    push_chat_limit = stream->readInt32(&error);
    saved_gifs_limit = stream->readInt32(&error);
    edit_time_limit = stream->readInt32(&error);
    revoke_time_limit = stream->readInt32(&error);
    revoke_pm_time_limit = stream->readInt32(&error);
    rating_e_decay = stream->readInt32(&error);
    stickers_recent_limit = stream->readInt32(&error);
    stickers_faved_limit = stream->readInt32(&error);
    channels_read_media_period = stream->readInt32(&error);
    if ((flags & 1) != 0) {
        tmp_sessions = stream->readInt32(&error);
    }
    pinned_dialogs_count_max = stream->readInt32(&error);
    pinned_infolder_count_max = stream->readInt32(&error);
    call_receive_timeout_ms = stream->readInt32(&error);
    call_ring_timeout_ms = stream->readInt32(&error);
    call_connect_timeout_ms = stream->readInt32(&error);
    call_packet_timeout_ms = stream->readInt32(&error);
    me_url_prefix = stream->readString(&error);
    if ((flags & 128) != 0) {
        autoupdate_url_prefix = stream->readString(&error);
    }
    if ((flags & 512) != 0) {
        gif_search_username = stream->readString(&error);
    }
    if ((flags & 1024) != 0) {
        venue_search_username = stream->readString(&error);
    }
    if ((flags & 2048) != 0) {
        img_search_username = stream->readString(&error);
    }
    if ((flags & 4096) != 0) {
        static_maps_provider = stream->readString(&error);
    }
    caption_length_max = stream->readInt32(&error);
    message_length_max = stream->readInt32(&error);
    webfile_dc_id = stream->readInt32(&error);
    if ((flags & 4) != 0) {
        suggested_lang_code = stream->readString(&error);
    }
    if ((flags & 4) != 0) {
        lang_pack_version = stream->readInt32(&error);
    }
    if ((flags & 4) != 0) {
        base_lang_pack_version = stream->readInt32(&error);
    }
}

void TL_config::serializeToStream(NativeByteBuffer *stream) {
    stream->writeInt32(constructor);
    stream->writeInt32(flags);
    stream->writeInt32(date);
    stream->writeInt32(expires);
    stream->writeBool(test_mode);
    stream->writeInt32(this_dc);
    stream->writeInt32(0x1cb5c415);
    uint32_t count = (uint32_t) dc_options.size();
    stream->writeInt32(count);
    for (uint32_t a = 0; a < count; a++) {
        dc_options[a]->serializeToStream(stream);
    }
    stream->writeString(dc_txt_domain_name);
    stream->writeInt32(chat_size_max);
    stream->writeInt32(megagroup_size_max);
    stream->writeInt32(forwarded_count_max);
    stream->writeInt32(online_update_period_ms);
    stream->writeInt32(offline_blur_timeout_ms);
    stream->writeInt32(offline_idle_timeout_ms);
    stream->writeInt32(online_cloud_timeout_ms);
    stream->writeInt32(notify_cloud_delay_ms);
    stream->writeInt32(notify_default_delay_ms);
    stream->writeInt32(push_chat_period_ms);
    stream->writeInt32(push_chat_limit);
    stream->writeInt32(saved_gifs_limit);
    stream->writeInt32(edit_time_limit);
    stream->writeInt32(revoke_time_limit);
    stream->writeInt32(revoke_pm_time_limit);
    stream->writeInt32(rating_e_decay);
    stream->writeInt32(stickers_recent_limit);
    stream->writeInt32(stickers_faved_limit);
    stream->writeInt32(channels_read_media_period);
    if ((flags & 1) != 0) {
        stream->writeInt32(tmp_sessions);
    }
    stream->writeInt32(pinned_dialogs_count_max);
    stream->writeInt32(pinned_infolder_count_max);
    stream->writeInt32(call_receive_timeout_ms);
    stream->writeInt32(call_ring_timeout_ms);
    stream->writeInt32(call_connect_timeout_ms);
    stream->writeInt32(call_packet_timeout_ms);
    stream->writeString(me_url_prefix);
    if ((flags & 128) != 0) {
        stream->writeString(autoupdate_url_prefix);
    }
    if ((flags & 512) != 0) {
        stream->writeString(gif_search_username);
    }
    if ((flags & 1024) != 0) {
        stream->writeString(venue_search_username);
    }
    if ((flags & 2048) != 0) {
        stream->writeString(img_search_username);
    }
    if ((flags & 4096) != 0) {
        stream->writeString(static_maps_provider);
    }
    stream->writeInt32(caption_length_max);
    stream->writeInt32(message_length_max);
    stream->writeInt32(webfile_dc_id);
    if ((flags & 4) != 0) {
        stream->writeString(suggested_lang_code);
    }
    if ((flags & 4) != 0) {
        stream->writeInt32(lang_pack_version);
    }
    if ((flags & 4) != 0) {
        stream->writeInt32(base_lang_pack_version);
    }
}

TLObject *TL_help_getConfig::deserializeResponse(NativeByteBuffer *stream, uint32_t constructor, int32_t instanceNum, bool &error) {
    return TL_config::TLdeserialize(stream, constructor, instanceNum, error);
}

void TL_help_getConfig::serializeToStream(NativeByteBuffer *stream) {
    stream->writeInt32(constructor);
}

bool TL_help_getConfig::isNeedLayer() {
    return true;
}

bool TL_account_registerDevice::isNeedLayer() {
    return true;
}

TLObject *TL_account_registerDevice::deserializeResponse(NativeByteBuffer *stream, uint32_t constructor, int32_t instanceNum, bool &error) {
    return Bool::TLdeserialize(stream, constructor, instanceNum, error);
}

void TL_account_registerDevice::serializeToStream(NativeByteBuffer *stream) {
    stream->writeInt32(constructor);
    stream->writeInt32(token_type);
    stream->writeString(token);
}

User *User::TLdeserialize(NativeByteBuffer *stream, uint32_t constructor, int32_t instanceNum, bool &error) {
    User *result = nullptr;
    switch (constructor) {
        case 0x200250ba:
            result = new TL_userEmpty();
            break;
        case 0x2e13f4c3:
            result = new TL_user();
            break;
        default:
            error = true;
            if (LOGS_ENABLED) DEBUG_E("can't parse magic %x in User", constructor);
            return nullptr;
    }
    result->readParams(stream, instanceNum, error);
    return result;
}

void TL_userEmpty::readParams(NativeByteBuffer *stream, int32_t instanceNum, bool &error) {
    id = stream->readInt32(&error);
}

void TL_userEmpty::serializeToStream(NativeByteBuffer *stream) {
    stream->writeInt32(constructor);
    stream->writeInt32(id);
}

void TL_user::readParams(NativeByteBuffer *stream, int32_t instanceNum, bool &error) {
    flags = stream->readInt32(&error);
    id = stream->readInt32(&error);
    if ((flags & 1) != 0) {
        access_hash = stream->readInt64(&error);
    }
    if ((flags & 2) != 0) {
        first_name = stream->readString(&error);
    }
    if ((flags & 4) != 0) {
        last_name = stream->readString(&error);
    }
    if ((flags & 8) != 0) {
        username = stream->readString(&error);
    }
    if ((flags & 16) != 0) {
        phone = stream->readString(&error);
    }
    if ((flags & 32) != 0) {
        photo = std::unique_ptr<UserProfilePhoto>(UserProfilePhoto::TLdeserialize(stream, stream->readUint32(&error), instanceNum, error));
    }
    if ((flags & 64) != 0) {
        status = std::unique_ptr<UserStatus>(UserStatus::TLdeserialize(stream, stream->readUint32(&error), instanceNum, error));
    }
    if ((flags & 16384) != 0) {
        bot_info_version = stream->readInt32(&error);
    }
    if ((flags & 262144) != 0) {
        restriction_reason = stream->readString(&error);
    }
    if ((flags & 524288) != 0) {
        bot_inline_placeholder = stream->readString(&error);
    }
    if ((flags & 4194304) != 0) {
        lang_code = stream->readString(&error);
    }
}

void TL_user::serializeToStream(NativeByteBuffer *stream) {
    stream->writeInt32(constructor);
    stream->writeInt32(flags);
    stream->writeInt32(id);
    if ((flags & 1) != 0) {
        stream->writeInt64(access_hash);
    }
    if ((flags & 2) != 0) {
        stream->writeString(first_name);
    }
    if ((flags & 4) != 0) {
        stream->writeString(last_name);
    }
    if ((flags & 8) != 0) {
        stream->writeString(username);
    }
    if ((flags & 16) != 0) {
        stream->writeString(phone);
    }
    if ((flags & 32) != 0) {
        photo->serializeToStream(stream);
    }
    if ((flags & 64) != 0) {
        status->serializeToStream(stream);
    }
    if ((flags & 16384) != 0) {
        stream->writeInt32(bot_info_version);
    }
    if ((flags & 262144) != 0) {
        stream->writeString(restriction_reason);
    }
    if ((flags & 524288) != 0) {
        stream->writeString(bot_inline_placeholder);
    }
    if ((flags & 4194304) != 0) {
        stream->writeString(lang_code);
    }
}

TL_auth_authorization *TL_auth_authorization::TLdeserialize(NativeByteBuffer *stream, uint32_t constructor, int32_t instanceNum, bool &error) {
    if (TL_auth_authorization::constructor != constructor) {
        error = true;
        if (LOGS_ENABLED) DEBUG_E("can't parse magic %x in TL_auth_authorization", constructor);
        return nullptr;
    }
    TL_auth_authorization *result = new TL_auth_authorization();
    result->readParams(stream, instanceNum, error);
    return result;
}

void TL_auth_authorization::readParams(NativeByteBuffer *stream, int32_t instanceNum, bool &error) {
    flags = stream->readInt32(&error);
    if ((flags & 1) != 0) {
        tmp_sessions = stream->readInt32(&error);
    }
    user = std::unique_ptr<User>(User::TLdeserialize(stream, stream->readUint32(&error), instanceNum, error));
}

TL_auth_exportedAuthorization *TL_auth_exportedAuthorization::TLdeserialize(NativeByteBuffer *stream, uint32_t constructor, int32_t instanceNum, bool &error) {
    if (TL_auth_exportedAuthorization::constructor != constructor) {
        error = true;
        if (LOGS_ENABLED) DEBUG_E("can't parse magic %x in TL_auth_exportedAuthorization", constructor);
        return nullptr;
    }
    TL_auth_exportedAuthorization *result = new TL_auth_exportedAuthorization();
    result->readParams(stream, instanceNum, error);
    return result;
}

void TL_auth_exportedAuthorization::readParams(NativeByteBuffer *stream, int32_t instanceNum, bool &error) {
    id = stream->readInt32(&error);
    bytes = std::unique_ptr<ByteArray>(stream->readByteArray(&error));
}

bool TL_auth_exportAuthorization::isNeedLayer() {
    return true;
}

TLObject *TL_auth_exportAuthorization::deserializeResponse(NativeByteBuffer *stream, uint32_t constructor, int32_t instanceNum, bool &error) {
    return TL_auth_exportedAuthorization::TLdeserialize(stream, constructor, instanceNum, error);
}

void TL_auth_exportAuthorization::serializeToStream(NativeByteBuffer *stream) {
    stream->writeInt32(constructor);
    stream->writeInt32(dc_id);
}

bool TL_auth_importAuthorization::isNeedLayer() {
    return true;
}

TLObject *TL_auth_importAuthorization::deserializeResponse(NativeByteBuffer *stream, uint32_t constructor, int32_t instanceNum, bool &error) {
    return TL_auth_authorization::TLdeserialize(stream, constructor, instanceNum, error);
}

void TL_auth_importAuthorization::serializeToStream(NativeByteBuffer *stream) {
    stream->writeInt32(constructor);
    stream->writeInt32(id);
    stream->writeByteArray(bytes.get());
}

UserStatus *UserStatus::TLdeserialize(NativeByteBuffer *stream, uint32_t constructor, int32_t instanceNum, bool &error) {
    UserStatus *result = nullptr;
    switch (constructor) {
        case 0x8c703f:
            result = new TL_userStatusOffline();
            break;
        case 0x7bf09fc:
            result = new TL_userStatusLastWeek();
            break;
        case 0x9d05049:
            result = new TL_userStatusEmpty();
            break;
        case 0x77ebc742:
            result = new TL_userStatusLastMonth();
            break;
        case 0xedb93949:
            result = new TL_userStatusOnline();
            break;
        case 0xe26f42f1:
            result = new TL_userStatusRecently();
            break;
        default:
            error = true;
            if (LOGS_ENABLED) DEBUG_E("can't parse magic %x in UserStatus", constructor);
            return nullptr;
    }
    result->readParams(stream, instanceNum, error);
    return result;
}

void TL_userStatusOffline::readParams(NativeByteBuffer *stream, int32_t instanceNum, bool &error) {
    expires = stream->readInt32(&error);
}

void TL_userStatusOffline::serializeToStream(NativeByteBuffer *stream) {
    stream->writeInt32(constructor);
    stream->writeInt32(expires);
}

void TL_userStatusLastWeek::serializeToStream(NativeByteBuffer *stream) {
    stream->writeInt32(constructor);
}

void TL_userStatusEmpty::serializeToStream(NativeByteBuffer *stream) {
    stream->writeInt32(constructor);
}

void TL_userStatusLastMonth::serializeToStream(NativeByteBuffer *stream) {
    stream->writeInt32(constructor);
}

void TL_userStatusOnline::readParams(NativeByteBuffer *stream, int32_t instanceNum, bool &error) {
    expires = stream->readInt32(&error);
}

void TL_userStatusOnline::serializeToStream(NativeByteBuffer *stream) {
    stream->writeInt32(constructor);
    stream->writeInt32(expires);
}

void TL_userStatusRecently::serializeToStream(NativeByteBuffer *stream) {
    stream->writeInt32(constructor);
}

FileLocation *FileLocation::TLdeserialize(NativeByteBuffer *stream, uint32_t constructor, int32_t instanceNum, bool &error) {
    FileLocation *result = nullptr;
    switch (constructor) {
        case 0xbc7fc6cd:
            result = new TL_fileLocationToBeDeprecated();
            break;
        default:
            error = true;
            if (LOGS_ENABLED) DEBUG_E("can't parse magic %x in FileLocation", constructor);
            return nullptr;
    }
    result->readParams(stream, instanceNum, error);
    return result;
}

void TL_fileLocationToBeDeprecated::readParams(NativeByteBuffer *stream, int32_t instanceNum, bool &error) {
    volume_id = stream->readInt64(&error);
    local_id = stream->readInt32(&error);
}

void TL_fileLocationToBeDeprecated::serializeToStream(NativeByteBuffer *stream) {
    stream->writeInt32(constructor);
    stream->writeInt64(volume_id);
    stream->writeInt32(local_id);
}

UserProfilePhoto *UserProfilePhoto::TLdeserialize(NativeByteBuffer *stream, uint32_t constructor, int32_t instanceNum, bool &error) {
    UserProfilePhoto *result = nullptr;
    switch (constructor) {
        case 0x4f11bae1:
            result = new TL_userProfilePhotoEmpty();
            break;
        case 0xecd75d8c:
            result = new TL_userProfilePhoto();
            break;
        default:
            error = true;
            if (LOGS_ENABLED) DEBUG_E("can't parse magic %x in UserProfilePhoto", constructor);
            return nullptr;
    }
    result->readParams(stream, instanceNum, error);
    return result;
}

void TL_userProfilePhotoEmpty::serializeToStream(NativeByteBuffer *stream) {
    stream->writeInt32(constructor);
}

void TL_userProfilePhoto::readParams(NativeByteBuffer *stream, int32_t instanceNum, bool &error) {
    photo_id = stream->readInt64(&error);
    photo_small = std::unique_ptr<FileLocation>(FileLocation::TLdeserialize(stream, stream->readUint32(&error), instanceNum, error));
    photo_big = std::unique_ptr<FileLocation>(FileLocation::TLdeserialize(stream, stream->readUint32(&error), instanceNum, error));
    dc_id = stream->readInt32(&error);
}

void TL_userProfilePhoto::serializeToStream(NativeByteBuffer *stream) {
    stream->writeInt32(constructor);
    stream->writeInt64(photo_id);
    photo_small->serializeToStream(stream);
    photo_big->serializeToStream(stream);
    stream->writeInt32(dc_id);
}

void TL_updatesTooLong::serializeToStream(NativeByteBuffer *stream) {
    stream->writeInt32(constructor);
}
