// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: flash_info.proto

#ifndef PROTOBUF_INCLUDED_flash_5finfo_2eproto
#define PROTOBUF_INCLUDED_flash_5finfo_2eproto

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3006001
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/message.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#define PROTOBUF_INTERNAL_EXPORT_protobuf_flash_5finfo_2eproto

namespace protobuf_flash_5finfo_2eproto
{
// Internal implementation detail -- do not use these members.
struct TableStruct
{
    static const ::google::protobuf::internal::ParseTableField entries[];
    static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
    static const ::google::protobuf::internal::ParseTable schema[1];
    static const ::google::protobuf::internal::FieldMetadata field_metadata[];
    static const ::google::protobuf::internal::SerializationTable serialization_table[];
    static const ::google::protobuf::uint32 offsets[];
};
void AddDescriptors();
}  // namespace protobuf_flash_5finfo_2eproto
namespace flash
{
class FlashCmdPkt;
class FlashCmdPktDefaultTypeInternal;
extern FlashCmdPktDefaultTypeInternal _FlashCmdPkt_default_instance_;
}  // namespace flash
namespace google
{
namespace protobuf
{
template <>
::flash::FlashCmdPkt* Arena::CreateMaybeMessage<::flash::FlashCmdPkt>(Arena*);
}  // namespace protobuf
}  // namespace google
namespace flash
{
enum PacketType
{
    kCtrlOut = 0,
    kCtrlIn = 1,
    kBulkOut = 2,
    kBulkIn = 3,
    kAttrPowerOn = 4,
    kAttrPowerOff = 5,
    kAttrPowerConfig = 6,
    kAttrGetPowerConfig = 7,
    kAttrShutDown = 8,
    PacketType_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
    PacketType_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
bool PacketType_IsValid(int value);
const PacketType PacketType_MIN = kCtrlOut;
const PacketType PacketType_MAX = kAttrShutDown;
const int PacketType_ARRAYSIZE = PacketType_MAX + 1;

const ::google::protobuf::EnumDescriptor* PacketType_descriptor();
inline const ::std::string& PacketType_Name(PacketType value)
{
    return ::google::protobuf::internal::NameOfEnum(PacketType_descriptor(), value);
}
inline bool PacketType_Parse(const ::std::string& name, PacketType* value)
{
    return ::google::protobuf::internal::ParseNamedEnum<PacketType>(PacketType_descriptor(), name, value);
}
// ===================================================================

class FlashCmdPkt
    : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:flash.FlashCmdPkt) */
{
   public:
    FlashCmdPkt();
    virtual ~FlashCmdPkt();

    FlashCmdPkt(const FlashCmdPkt& from);

    inline FlashCmdPkt& operator=(const FlashCmdPkt& from)
    {
        CopyFrom(from);
        return *this;
    }
#if LANG_CXX11
    FlashCmdPkt(FlashCmdPkt&& from) noexcept : FlashCmdPkt() { *this = ::std::move(from); }

    inline FlashCmdPkt& operator=(FlashCmdPkt&& from) noexcept
    {
        if (GetArenaNoVirtual() == from.GetArenaNoVirtual())
        {
            if (this != &from) InternalSwap(&from);
        }
        else
        {
            CopyFrom(from);
        }
        return *this;
    }
#endif
    static const ::google::protobuf::Descriptor* descriptor();
    static const FlashCmdPkt& default_instance();

    static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
    static inline const FlashCmdPkt* internal_default_instance()
    {
        return reinterpret_cast<const FlashCmdPkt*>(&_FlashCmdPkt_default_instance_);
    }
    static constexpr int kIndexInFileMessages = 0;

    void Swap(FlashCmdPkt* other);
    friend void swap(FlashCmdPkt& a, FlashCmdPkt& b) { a.Swap(&b); }

    // implements Message ----------------------------------------------

    inline FlashCmdPkt* New() const final { return CreateMaybeMessage<FlashCmdPkt>(NULL); }

    FlashCmdPkt* New(::google::protobuf::Arena* arena) const final { return CreateMaybeMessage<FlashCmdPkt>(arena); }
    void CopyFrom(const ::google::protobuf::Message& from) final;
    void MergeFrom(const ::google::protobuf::Message& from) final;
    void CopyFrom(const FlashCmdPkt& from);
    void MergeFrom(const FlashCmdPkt& from);
    void Clear() final;
    bool IsInitialized() const final;

    size_t ByteSizeLong() const final;
    bool MergePartialFromCodedStream(::google::protobuf::io::CodedInputStream* input) final;
    void SerializeWithCachedSizes(::google::protobuf::io::CodedOutputStream* output) const final;
    ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(bool deterministic,
                                                                       ::google::protobuf::uint8* target) const final;
    int GetCachedSize() const final { return _cached_size_.Get(); }

   private:
    void SharedCtor();
    void SharedDtor();
    void SetCachedSize(int size) const final;
    void InternalSwap(FlashCmdPkt* other);

   private:
    inline ::google::protobuf::Arena* GetArenaNoVirtual() const { return NULL; }
    inline void* MaybeArenaPtr() const { return NULL; }

   public:
    ::google::protobuf::Metadata GetMetadata() const final;

    // nested types ----------------------------------------------------

    // accessors -------------------------------------------------------

    // bytes data = 2;
    void clear_data();
    static const int kDataFieldNumber = 2;
    const ::std::string& data() const;
    void set_data(const ::std::string& value);
#if LANG_CXX11
    void set_data(::std::string&& value);
#endif
    void set_data(const char* value);
    void set_data(const void* value, size_t size);
    ::std::string* mutable_data();
    ::std::string* release_data();
    void set_allocated_data(::std::string* data);

    // .flash.PacketType type = 1;
    void clear_type();
    static const int kTypeFieldNumber = 1;
    ::flash::PacketType type() const;
    void set_type(::flash::PacketType value);

    // bool cs = 4;
    void clear_cs();
    static const int kCsFieldNumber = 4;
    bool cs() const;
    void set_cs(bool value);

    // uint64 attribute = 3;
    void clear_attribute();
    static const int kAttributeFieldNumber = 3;
    ::google::protobuf::uint64 attribute() const;
    void set_attribute(::google::protobuf::uint64 value);

    // @@protoc_insertion_point(class_scope:flash.FlashCmdPkt)
   private:
    ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
    ::google::protobuf::internal::ArenaStringPtr data_;
    int type_;
    bool cs_;
    ::google::protobuf::uint64 attribute_;
    mutable ::google::protobuf::internal::CachedSize _cached_size_;
    friend struct ::protobuf_flash_5finfo_2eproto::TableStruct;
};
// ===================================================================

// ===================================================================

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// FlashCmdPkt

// .flash.PacketType type = 1;
inline void FlashCmdPkt::clear_type() { type_ = 0; }
inline ::flash::PacketType FlashCmdPkt::type() const
{
    // @@protoc_insertion_point(field_get:flash.FlashCmdPkt.type)
    return static_cast<::flash::PacketType>(type_);
}
inline void FlashCmdPkt::set_type(::flash::PacketType value)
{
    type_ = value;
    // @@protoc_insertion_point(field_set:flash.FlashCmdPkt.type)
}

// bytes data = 2;
inline void FlashCmdPkt::clear_data()
{
    data_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& FlashCmdPkt::data() const
{
    // @@protoc_insertion_point(field_get:flash.FlashCmdPkt.data)
    return data_.GetNoArena();
}
inline void FlashCmdPkt::set_data(const ::std::string& value)
{
    data_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
    // @@protoc_insertion_point(field_set:flash.FlashCmdPkt.data)
}
#if LANG_CXX11
inline void FlashCmdPkt::set_data(::std::string&& value)
{
    data_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
    // @@protoc_insertion_point(field_set_rvalue:flash.FlashCmdPkt.data)
}
#endif
inline void FlashCmdPkt::set_data(const char* value)
{
    GOOGLE_DCHECK(value != NULL);

    data_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
    // @@protoc_insertion_point(field_set_char:flash.FlashCmdPkt.data)
}
inline void FlashCmdPkt::set_data(const void* value, size_t size)
{
    data_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
                     ::std::string(reinterpret_cast<const char*>(value), size));
    // @@protoc_insertion_point(field_set_pointer:flash.FlashCmdPkt.data)
}
inline ::std::string* FlashCmdPkt::mutable_data()
{
    // @@protoc_insertion_point(field_mutable:flash.FlashCmdPkt.data)
    return data_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* FlashCmdPkt::release_data()
{
    // @@protoc_insertion_point(field_release:flash.FlashCmdPkt.data)

    return data_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void FlashCmdPkt::set_allocated_data(::std::string* data)
{
    if (data != NULL)
    {
    }
    else
    {
    }
    data_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), data);
    // @@protoc_insertion_point(field_set_allocated:flash.FlashCmdPkt.data)
}

// uint64 attribute = 3;
inline void FlashCmdPkt::clear_attribute() { attribute_ = GOOGLE_ULONGLONG(0); }
inline ::google::protobuf::uint64 FlashCmdPkt::attribute() const
{
    // @@protoc_insertion_point(field_get:flash.FlashCmdPkt.attribute)
    return attribute_;
}
inline void FlashCmdPkt::set_attribute(::google::protobuf::uint64 value)
{
    attribute_ = value;
    // @@protoc_insertion_point(field_set:flash.FlashCmdPkt.attribute)
}

// bool cs = 4;
inline void FlashCmdPkt::clear_cs() { cs_ = false; }
inline bool FlashCmdPkt::cs() const
{
    // @@protoc_insertion_point(field_get:flash.FlashCmdPkt.cs)
    return cs_;
}
inline void FlashCmdPkt::set_cs(bool value)
{
    cs_ = value;
    // @@protoc_insertion_point(field_set:flash.FlashCmdPkt.cs)
}

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace flash

namespace google
{
namespace protobuf
{
template <>
struct is_proto_enum<::flash::PacketType> : ::std::true_type
{
};
template <>
inline const EnumDescriptor* GetEnumDescriptor<::flash::PacketType>()
{
    return ::flash::PacketType_descriptor();
}

}  // namespace protobuf
}  // namespace google

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_INCLUDED_flash_5finfo_2eproto
