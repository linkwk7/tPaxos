// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: message.proto

#ifndef PROTOBUF_message_2eproto__INCLUDED
#define PROTOBUF_message_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3005000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3005001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace protobuf_message_2eproto {
// Internal implementation detail -- do not use these members.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[1];
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static const ::google::protobuf::uint32 offsets[];
};
void AddDescriptors();
void InitDefaultsPaxosMsgImpl();
void InitDefaultsPaxosMsg();
inline void InitDefaults() {
  InitDefaultsPaxosMsg();
}
}  // namespace protobuf_message_2eproto
namespace tPaxos {
class PaxosMsg;
class PaxosMsgDefaultTypeInternal;
extern PaxosMsgDefaultTypeInternal _PaxosMsg_default_instance_;
}  // namespace tPaxos
namespace tPaxos {

// ===================================================================

class PaxosMsg : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:tPaxos.PaxosMsg) */ {
 public:
  PaxosMsg();
  virtual ~PaxosMsg();

  PaxosMsg(const PaxosMsg& from);

  inline PaxosMsg& operator=(const PaxosMsg& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  PaxosMsg(PaxosMsg&& from) noexcept
    : PaxosMsg() {
    *this = ::std::move(from);
  }

  inline PaxosMsg& operator=(PaxosMsg&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _internal_metadata_.unknown_fields();
  }
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields();
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const PaxosMsg& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const PaxosMsg* internal_default_instance() {
    return reinterpret_cast<const PaxosMsg*>(
               &_PaxosMsg_default_instance_);
  }
  static PROTOBUF_CONSTEXPR int const kIndexInFileMessages =
    0;

  void Swap(PaxosMsg* other);
  friend void swap(PaxosMsg& a, PaxosMsg& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline PaxosMsg* New() const PROTOBUF_FINAL { return New(NULL); }

  PaxosMsg* New(::google::protobuf::Arena* arena) const PROTOBUF_FINAL;
  void CopyFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void MergeFrom(const ::google::protobuf::Message& from) PROTOBUF_FINAL;
  void CopyFrom(const PaxosMsg& from);
  void MergeFrom(const PaxosMsg& from);
  void Clear() PROTOBUF_FINAL;
  bool IsInitialized() const PROTOBUF_FINAL;

  size_t ByteSizeLong() const PROTOBUF_FINAL;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) PROTOBUF_FINAL;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const PROTOBUF_FINAL;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const PROTOBUF_FINAL;
  int GetCachedSize() const PROTOBUF_FINAL { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const PROTOBUF_FINAL;
  void InternalSwap(PaxosMsg* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const PROTOBUF_FINAL;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional bytes Value = 6;
  bool has_value() const;
  void clear_value();
  static const int kValueFieldNumber = 6;
  const ::std::string& value() const;
  void set_value(const ::std::string& value);
  #if LANG_CXX11
  void set_value(::std::string&& value);
  #endif
  void set_value(const char* value);
  void set_value(const void* value, size_t size);
  ::std::string* mutable_value();
  ::std::string* release_value();
  void set_allocated_value(::std::string* value);

  // required uint32 GLType = 1;
  bool has_gltype() const;
  void clear_gltype();
  static const int kGLTypeFieldNumber = 1;
  ::google::protobuf::uint32 gltype() const;
  void set_gltype(::google::protobuf::uint32 value);

  // optional uint32 InstanceID = 2;
  bool has_instanceid() const;
  void clear_instanceid();
  static const int kInstanceIDFieldNumber = 2;
  ::google::protobuf::uint32 instanceid() const;
  void set_instanceid(::google::protobuf::uint32 value);

  // optional uint32 NodeID = 3;
  bool has_nodeid() const;
  void clear_nodeid();
  static const int kNodeIDFieldNumber = 3;
  ::google::protobuf::uint32 nodeid() const;
  void set_nodeid(::google::protobuf::uint32 value);

  // optional uint32 ILType = 4;
  bool has_iltype() const;
  void clear_iltype();
  static const int kILTypeFieldNumber = 4;
  ::google::protobuf::uint32 iltype() const;
  void set_iltype(::google::protobuf::uint32 value);

  // optional uint32 ProposalID = 5;
  bool has_proposalid() const;
  void clear_proposalid();
  static const int kProposalIDFieldNumber = 5;
  ::google::protobuf::uint32 proposalid() const;
  void set_proposalid(::google::protobuf::uint32 value);

  // optional uint32 AcceptedProposalID = 7;
  bool has_acceptedproposalid() const;
  void clear_acceptedproposalid();
  static const int kAcceptedProposalIDFieldNumber = 7;
  ::google::protobuf::uint32 acceptedproposalid() const;
  void set_acceptedproposalid(::google::protobuf::uint32 value);

  // optional uint32 AcceptedNodeID = 8;
  bool has_acceptednodeid() const;
  void clear_acceptednodeid();
  static const int kAcceptedNodeIDFieldNumber = 8;
  ::google::protobuf::uint32 acceptednodeid() const;
  void set_acceptednodeid(::google::protobuf::uint32 value);

  // optional bool Reject = 9;
  bool has_reject() const;
  void clear_reject();
  static const int kRejectFieldNumber = 9;
  bool reject() const;
  void set_reject(bool value);

  // optional uint32 PromisedProposalID = 10;
  bool has_promisedproposalid() const;
  void clear_promisedproposalid();
  static const int kPromisedProposalIDFieldNumber = 10;
  ::google::protobuf::uint32 promisedproposalid() const;
  void set_promisedproposalid(::google::protobuf::uint32 value);

  // @@protoc_insertion_point(class_scope:tPaxos.PaxosMsg)
 private:
  void set_has_gltype();
  void clear_has_gltype();
  void set_has_instanceid();
  void clear_has_instanceid();
  void set_has_nodeid();
  void clear_has_nodeid();
  void set_has_iltype();
  void clear_has_iltype();
  void set_has_proposalid();
  void clear_has_proposalid();
  void set_has_value();
  void clear_has_value();
  void set_has_acceptedproposalid();
  void clear_has_acceptedproposalid();
  void set_has_acceptednodeid();
  void clear_has_acceptednodeid();
  void set_has_reject();
  void clear_has_reject();
  void set_has_promisedproposalid();
  void clear_has_promisedproposalid();

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::HasBits<1> _has_bits_;
  mutable int _cached_size_;
  ::google::protobuf::internal::ArenaStringPtr value_;
  ::google::protobuf::uint32 gltype_;
  ::google::protobuf::uint32 instanceid_;
  ::google::protobuf::uint32 nodeid_;
  ::google::protobuf::uint32 iltype_;
  ::google::protobuf::uint32 proposalid_;
  ::google::protobuf::uint32 acceptedproposalid_;
  ::google::protobuf::uint32 acceptednodeid_;
  bool reject_;
  ::google::protobuf::uint32 promisedproposalid_;
  friend struct ::protobuf_message_2eproto::TableStruct;
  friend void ::protobuf_message_2eproto::InitDefaultsPaxosMsgImpl();
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// PaxosMsg

// required uint32 GLType = 1;
inline bool PaxosMsg::has_gltype() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void PaxosMsg::set_has_gltype() {
  _has_bits_[0] |= 0x00000002u;
}
inline void PaxosMsg::clear_has_gltype() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void PaxosMsg::clear_gltype() {
  gltype_ = 0u;
  clear_has_gltype();
}
inline ::google::protobuf::uint32 PaxosMsg::gltype() const {
  // @@protoc_insertion_point(field_get:tPaxos.PaxosMsg.GLType)
  return gltype_;
}
inline void PaxosMsg::set_gltype(::google::protobuf::uint32 value) {
  set_has_gltype();
  gltype_ = value;
  // @@protoc_insertion_point(field_set:tPaxos.PaxosMsg.GLType)
}

// optional uint32 InstanceID = 2;
inline bool PaxosMsg::has_instanceid() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void PaxosMsg::set_has_instanceid() {
  _has_bits_[0] |= 0x00000004u;
}
inline void PaxosMsg::clear_has_instanceid() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void PaxosMsg::clear_instanceid() {
  instanceid_ = 0u;
  clear_has_instanceid();
}
inline ::google::protobuf::uint32 PaxosMsg::instanceid() const {
  // @@protoc_insertion_point(field_get:tPaxos.PaxosMsg.InstanceID)
  return instanceid_;
}
inline void PaxosMsg::set_instanceid(::google::protobuf::uint32 value) {
  set_has_instanceid();
  instanceid_ = value;
  // @@protoc_insertion_point(field_set:tPaxos.PaxosMsg.InstanceID)
}

// optional uint32 NodeID = 3;
inline bool PaxosMsg::has_nodeid() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void PaxosMsg::set_has_nodeid() {
  _has_bits_[0] |= 0x00000008u;
}
inline void PaxosMsg::clear_has_nodeid() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void PaxosMsg::clear_nodeid() {
  nodeid_ = 0u;
  clear_has_nodeid();
}
inline ::google::protobuf::uint32 PaxosMsg::nodeid() const {
  // @@protoc_insertion_point(field_get:tPaxos.PaxosMsg.NodeID)
  return nodeid_;
}
inline void PaxosMsg::set_nodeid(::google::protobuf::uint32 value) {
  set_has_nodeid();
  nodeid_ = value;
  // @@protoc_insertion_point(field_set:tPaxos.PaxosMsg.NodeID)
}

// optional uint32 ILType = 4;
inline bool PaxosMsg::has_iltype() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void PaxosMsg::set_has_iltype() {
  _has_bits_[0] |= 0x00000010u;
}
inline void PaxosMsg::clear_has_iltype() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void PaxosMsg::clear_iltype() {
  iltype_ = 0u;
  clear_has_iltype();
}
inline ::google::protobuf::uint32 PaxosMsg::iltype() const {
  // @@protoc_insertion_point(field_get:tPaxos.PaxosMsg.ILType)
  return iltype_;
}
inline void PaxosMsg::set_iltype(::google::protobuf::uint32 value) {
  set_has_iltype();
  iltype_ = value;
  // @@protoc_insertion_point(field_set:tPaxos.PaxosMsg.ILType)
}

// optional uint32 ProposalID = 5;
inline bool PaxosMsg::has_proposalid() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void PaxosMsg::set_has_proposalid() {
  _has_bits_[0] |= 0x00000020u;
}
inline void PaxosMsg::clear_has_proposalid() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void PaxosMsg::clear_proposalid() {
  proposalid_ = 0u;
  clear_has_proposalid();
}
inline ::google::protobuf::uint32 PaxosMsg::proposalid() const {
  // @@protoc_insertion_point(field_get:tPaxos.PaxosMsg.ProposalID)
  return proposalid_;
}
inline void PaxosMsg::set_proposalid(::google::protobuf::uint32 value) {
  set_has_proposalid();
  proposalid_ = value;
  // @@protoc_insertion_point(field_set:tPaxos.PaxosMsg.ProposalID)
}

// optional bytes Value = 6;
inline bool PaxosMsg::has_value() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void PaxosMsg::set_has_value() {
  _has_bits_[0] |= 0x00000001u;
}
inline void PaxosMsg::clear_has_value() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void PaxosMsg::clear_value() {
  value_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  clear_has_value();
}
inline const ::std::string& PaxosMsg::value() const {
  // @@protoc_insertion_point(field_get:tPaxos.PaxosMsg.Value)
  return value_.GetNoArena();
}
inline void PaxosMsg::set_value(const ::std::string& value) {
  set_has_value();
  value_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:tPaxos.PaxosMsg.Value)
}
#if LANG_CXX11
inline void PaxosMsg::set_value(::std::string&& value) {
  set_has_value();
  value_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:tPaxos.PaxosMsg.Value)
}
#endif
inline void PaxosMsg::set_value(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  set_has_value();
  value_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:tPaxos.PaxosMsg.Value)
}
inline void PaxosMsg::set_value(const void* value, size_t size) {
  set_has_value();
  value_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:tPaxos.PaxosMsg.Value)
}
inline ::std::string* PaxosMsg::mutable_value() {
  set_has_value();
  // @@protoc_insertion_point(field_mutable:tPaxos.PaxosMsg.Value)
  return value_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* PaxosMsg::release_value() {
  // @@protoc_insertion_point(field_release:tPaxos.PaxosMsg.Value)
  clear_has_value();
  return value_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void PaxosMsg::set_allocated_value(::std::string* value) {
  if (value != NULL) {
    set_has_value();
  } else {
    clear_has_value();
  }
  value_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set_allocated:tPaxos.PaxosMsg.Value)
}

// optional uint32 AcceptedProposalID = 7;
inline bool PaxosMsg::has_acceptedproposalid() const {
  return (_has_bits_[0] & 0x00000040u) != 0;
}
inline void PaxosMsg::set_has_acceptedproposalid() {
  _has_bits_[0] |= 0x00000040u;
}
inline void PaxosMsg::clear_has_acceptedproposalid() {
  _has_bits_[0] &= ~0x00000040u;
}
inline void PaxosMsg::clear_acceptedproposalid() {
  acceptedproposalid_ = 0u;
  clear_has_acceptedproposalid();
}
inline ::google::protobuf::uint32 PaxosMsg::acceptedproposalid() const {
  // @@protoc_insertion_point(field_get:tPaxos.PaxosMsg.AcceptedProposalID)
  return acceptedproposalid_;
}
inline void PaxosMsg::set_acceptedproposalid(::google::protobuf::uint32 value) {
  set_has_acceptedproposalid();
  acceptedproposalid_ = value;
  // @@protoc_insertion_point(field_set:tPaxos.PaxosMsg.AcceptedProposalID)
}

// optional uint32 AcceptedNodeID = 8;
inline bool PaxosMsg::has_acceptednodeid() const {
  return (_has_bits_[0] & 0x00000080u) != 0;
}
inline void PaxosMsg::set_has_acceptednodeid() {
  _has_bits_[0] |= 0x00000080u;
}
inline void PaxosMsg::clear_has_acceptednodeid() {
  _has_bits_[0] &= ~0x00000080u;
}
inline void PaxosMsg::clear_acceptednodeid() {
  acceptednodeid_ = 0u;
  clear_has_acceptednodeid();
}
inline ::google::protobuf::uint32 PaxosMsg::acceptednodeid() const {
  // @@protoc_insertion_point(field_get:tPaxos.PaxosMsg.AcceptedNodeID)
  return acceptednodeid_;
}
inline void PaxosMsg::set_acceptednodeid(::google::protobuf::uint32 value) {
  set_has_acceptednodeid();
  acceptednodeid_ = value;
  // @@protoc_insertion_point(field_set:tPaxos.PaxosMsg.AcceptedNodeID)
}

// optional bool Reject = 9;
inline bool PaxosMsg::has_reject() const {
  return (_has_bits_[0] & 0x00000100u) != 0;
}
inline void PaxosMsg::set_has_reject() {
  _has_bits_[0] |= 0x00000100u;
}
inline void PaxosMsg::clear_has_reject() {
  _has_bits_[0] &= ~0x00000100u;
}
inline void PaxosMsg::clear_reject() {
  reject_ = false;
  clear_has_reject();
}
inline bool PaxosMsg::reject() const {
  // @@protoc_insertion_point(field_get:tPaxos.PaxosMsg.Reject)
  return reject_;
}
inline void PaxosMsg::set_reject(bool value) {
  set_has_reject();
  reject_ = value;
  // @@protoc_insertion_point(field_set:tPaxos.PaxosMsg.Reject)
}

// optional uint32 PromisedProposalID = 10;
inline bool PaxosMsg::has_promisedproposalid() const {
  return (_has_bits_[0] & 0x00000200u) != 0;
}
inline void PaxosMsg::set_has_promisedproposalid() {
  _has_bits_[0] |= 0x00000200u;
}
inline void PaxosMsg::clear_has_promisedproposalid() {
  _has_bits_[0] &= ~0x00000200u;
}
inline void PaxosMsg::clear_promisedproposalid() {
  promisedproposalid_ = 0u;
  clear_has_promisedproposalid();
}
inline ::google::protobuf::uint32 PaxosMsg::promisedproposalid() const {
  // @@protoc_insertion_point(field_get:tPaxos.PaxosMsg.PromisedProposalID)
  return promisedproposalid_;
}
inline void PaxosMsg::set_promisedproposalid(::google::protobuf::uint32 value) {
  set_has_promisedproposalid();
  promisedproposalid_ = value;
  // @@protoc_insertion_point(field_set:tPaxos.PaxosMsg.PromisedProposalID)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace tPaxos

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_message_2eproto__INCLUDED