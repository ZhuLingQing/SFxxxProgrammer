// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: hal_proto.proto

#ifndef PROTOBUF_INCLUDED_hal_5fproto_2eproto
#define PROTOBUF_INCLUDED_hal_5fproto_2eproto

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

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#define PROTOBUF_INTERNAL_EXPORT_protobuf_hal_5fproto_2eproto 

namespace protobuf_hal_5fproto_2eproto {
// Internal implementation detail -- do not use these members.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[2];
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static const ::google::protobuf::uint32 offsets[];
};
void AddDescriptors();
}  // namespace protobuf_hal_5fproto_2eproto
namespace proto_hal {
class HalSktCtrlPacket;
class HalSktCtrlPacketDefaultTypeInternal;
extern HalSktCtrlPacketDefaultTypeInternal _HalSktCtrlPacket_default_instance_;
class HalSktDataPacket;
class HalSktDataPacketDefaultTypeInternal;
extern HalSktDataPacketDefaultTypeInternal _HalSktDataPacket_default_instance_;
}  // namespace proto_hal
namespace google {
namespace protobuf {
template<> ::proto_hal::HalSktCtrlPacket* Arena::CreateMaybeMessage<::proto_hal::HalSktCtrlPacket>(Arena*);
template<> ::proto_hal::HalSktDataPacket* Arena::CreateMaybeMessage<::proto_hal::HalSktDataPacket>(Arena*);
}  // namespace protobuf
}  // namespace google
namespace proto_hal {

enum PacketType {
  kCtrlOut = 0,
  kCtrlIn = 1,
  kBulkOut = 2,
  kBulkIn = 3,
  kShutdown = 4,
  PacketType_INT_MIN_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32min,
  PacketType_INT_MAX_SENTINEL_DO_NOT_USE_ = ::google::protobuf::kint32max
};
bool PacketType_IsValid(int value);
const PacketType PacketType_MIN = kCtrlOut;
const PacketType PacketType_MAX = kShutdown;
const int PacketType_ARRAYSIZE = PacketType_MAX + 1;

const ::google::protobuf::EnumDescriptor* PacketType_descriptor();
inline const ::std::string& PacketType_Name(PacketType value) {
  return ::google::protobuf::internal::NameOfEnum(
    PacketType_descriptor(), value);
}
inline bool PacketType_Parse(
    const ::std::string& name, PacketType* value) {
  return ::google::protobuf::internal::ParseNamedEnum<PacketType>(
    PacketType_descriptor(), name, value);
}
// ===================================================================

class HalSktCtrlPacket : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:proto_hal.HalSktCtrlPacket) */ {
 public:
  HalSktCtrlPacket();
  virtual ~HalSktCtrlPacket();

  HalSktCtrlPacket(const HalSktCtrlPacket& from);

  inline HalSktCtrlPacket& operator=(const HalSktCtrlPacket& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  HalSktCtrlPacket(HalSktCtrlPacket&& from) noexcept
    : HalSktCtrlPacket() {
    *this = ::std::move(from);
  }

  inline HalSktCtrlPacket& operator=(HalSktCtrlPacket&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const HalSktCtrlPacket& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const HalSktCtrlPacket* internal_default_instance() {
    return reinterpret_cast<const HalSktCtrlPacket*>(
               &_HalSktCtrlPacket_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  void Swap(HalSktCtrlPacket* other);
  friend void swap(HalSktCtrlPacket& a, HalSktCtrlPacket& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline HalSktCtrlPacket* New() const final {
    return CreateMaybeMessage<HalSktCtrlPacket>(NULL);
  }

  HalSktCtrlPacket* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<HalSktCtrlPacket>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const HalSktCtrlPacket& from);
  void MergeFrom(const HalSktCtrlPacket& from);
  void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(HalSktCtrlPacket* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // bytes payload = 3;
  void clear_payload();
  static const int kPayloadFieldNumber = 3;
  const ::std::string& payload() const;
  void set_payload(const ::std::string& value);
  #if LANG_CXX11
  void set_payload(::std::string&& value);
  #endif
  void set_payload(const char* value);
  void set_payload(const void* value, size_t size);
  ::std::string* mutable_payload();
  ::std::string* release_payload();
  void set_allocated_payload(::std::string* payload);

  // uint64 ctrl_word = 2;
  void clear_ctrl_word();
  static const int kCtrlWordFieldNumber = 2;
  ::google::protobuf::uint64 ctrl_word() const;
  void set_ctrl_word(::google::protobuf::uint64 value);

  // .proto_hal.PacketType type = 1;
  void clear_type();
  static const int kTypeFieldNumber = 1;
  ::proto_hal::PacketType type() const;
  void set_type(::proto_hal::PacketType value);

  // @@protoc_insertion_point(class_scope:proto_hal.HalSktCtrlPacket)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::ArenaStringPtr payload_;
  ::google::protobuf::uint64 ctrl_word_;
  int type_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::protobuf_hal_5fproto_2eproto::TableStruct;
};
// -------------------------------------------------------------------

class HalSktDataPacket : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:proto_hal.HalSktDataPacket) */ {
 public:
  HalSktDataPacket();
  virtual ~HalSktDataPacket();

  HalSktDataPacket(const HalSktDataPacket& from);

  inline HalSktDataPacket& operator=(const HalSktDataPacket& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  HalSktDataPacket(HalSktDataPacket&& from) noexcept
    : HalSktDataPacket() {
    *this = ::std::move(from);
  }

  inline HalSktDataPacket& operator=(HalSktDataPacket&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const HalSktDataPacket& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const HalSktDataPacket* internal_default_instance() {
    return reinterpret_cast<const HalSktDataPacket*>(
               &_HalSktDataPacket_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  void Swap(HalSktDataPacket* other);
  friend void swap(HalSktDataPacket& a, HalSktDataPacket& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline HalSktDataPacket* New() const final {
    return CreateMaybeMessage<HalSktDataPacket>(NULL);
  }

  HalSktDataPacket* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<HalSktDataPacket>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const HalSktDataPacket& from);
  void MergeFrom(const HalSktDataPacket& from);
  void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(HalSktDataPacket* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // bytes data = 1;
  void clear_data();
  static const int kDataFieldNumber = 1;
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

  // @@protoc_insertion_point(class_scope:proto_hal.HalSktDataPacket)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::ArenaStringPtr data_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::protobuf_hal_5fproto_2eproto::TableStruct;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// HalSktCtrlPacket

// .proto_hal.PacketType type = 1;
inline void HalSktCtrlPacket::clear_type() {
  type_ = 0;
}
inline ::proto_hal::PacketType HalSktCtrlPacket::type() const {
  // @@protoc_insertion_point(field_get:proto_hal.HalSktCtrlPacket.type)
  return static_cast< ::proto_hal::PacketType >(type_);
}
inline void HalSktCtrlPacket::set_type(::proto_hal::PacketType value) {
  
  type_ = value;
  // @@protoc_insertion_point(field_set:proto_hal.HalSktCtrlPacket.type)
}

// uint64 ctrl_word = 2;
inline void HalSktCtrlPacket::clear_ctrl_word() {
  ctrl_word_ = GOOGLE_ULONGLONG(0);
}
inline ::google::protobuf::uint64 HalSktCtrlPacket::ctrl_word() const {
  // @@protoc_insertion_point(field_get:proto_hal.HalSktCtrlPacket.ctrl_word)
  return ctrl_word_;
}
inline void HalSktCtrlPacket::set_ctrl_word(::google::protobuf::uint64 value) {
  
  ctrl_word_ = value;
  // @@protoc_insertion_point(field_set:proto_hal.HalSktCtrlPacket.ctrl_word)
}

// bytes payload = 3;
inline void HalSktCtrlPacket::clear_payload() {
  payload_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& HalSktCtrlPacket::payload() const {
  // @@protoc_insertion_point(field_get:proto_hal.HalSktCtrlPacket.payload)
  return payload_.GetNoArena();
}
inline void HalSktCtrlPacket::set_payload(const ::std::string& value) {
  
  payload_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:proto_hal.HalSktCtrlPacket.payload)
}
#if LANG_CXX11
inline void HalSktCtrlPacket::set_payload(::std::string&& value) {
  
  payload_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:proto_hal.HalSktCtrlPacket.payload)
}
#endif
inline void HalSktCtrlPacket::set_payload(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  payload_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:proto_hal.HalSktCtrlPacket.payload)
}
inline void HalSktCtrlPacket::set_payload(const void* value, size_t size) {
  
  payload_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:proto_hal.HalSktCtrlPacket.payload)
}
inline ::std::string* HalSktCtrlPacket::mutable_payload() {
  
  // @@protoc_insertion_point(field_mutable:proto_hal.HalSktCtrlPacket.payload)
  return payload_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* HalSktCtrlPacket::release_payload() {
  // @@protoc_insertion_point(field_release:proto_hal.HalSktCtrlPacket.payload)
  
  return payload_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void HalSktCtrlPacket::set_allocated_payload(::std::string* payload) {
  if (payload != NULL) {
    
  } else {
    
  }
  payload_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), payload);
  // @@protoc_insertion_point(field_set_allocated:proto_hal.HalSktCtrlPacket.payload)
}

// -------------------------------------------------------------------

// HalSktDataPacket

// bytes data = 1;
inline void HalSktDataPacket::clear_data() {
  data_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& HalSktDataPacket::data() const {
  // @@protoc_insertion_point(field_get:proto_hal.HalSktDataPacket.data)
  return data_.GetNoArena();
}
inline void HalSktDataPacket::set_data(const ::std::string& value) {
  
  data_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:proto_hal.HalSktDataPacket.data)
}
#if LANG_CXX11
inline void HalSktDataPacket::set_data(::std::string&& value) {
  
  data_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:proto_hal.HalSktDataPacket.data)
}
#endif
inline void HalSktDataPacket::set_data(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  data_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:proto_hal.HalSktDataPacket.data)
}
inline void HalSktDataPacket::set_data(const void* value, size_t size) {
  
  data_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:proto_hal.HalSktDataPacket.data)
}
inline ::std::string* HalSktDataPacket::mutable_data() {
  
  // @@protoc_insertion_point(field_mutable:proto_hal.HalSktDataPacket.data)
  return data_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* HalSktDataPacket::release_data() {
  // @@protoc_insertion_point(field_release:proto_hal.HalSktDataPacket.data)
  
  return data_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void HalSktDataPacket::set_allocated_data(::std::string* data) {
  if (data != NULL) {
    
  } else {
    
  }
  data_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), data);
  // @@protoc_insertion_point(field_set_allocated:proto_hal.HalSktDataPacket.data)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace proto_hal

namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::proto_hal::PacketType> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::proto_hal::PacketType>() {
  return ::proto_hal::PacketType_descriptor();
}

}  // namespace protobuf
}  // namespace google

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_INCLUDED_hal_5fproto_2eproto
