/**
 * Autogenerated by Thrift Compiler (0.10.0)
 *
 * DO NOT EDIT UNLESS YOU ARE SURE THAT YOU KNOW WHAT YOU ARE DOING
 *  @generated
 */
#ifndef RadiusPairLogger_TYPES_H
#define RadiusPairLogger_TYPES_H

#include <iosfwd>

#include <thrift/Thrift.h>
#include <thrift/TApplicationException.h>
#include <thrift/TBase.h>
#include <thrift/protocol/TProtocol.h>
#include <thrift/transport/TTransport.h>

#include <thrift/cxxfunctional.h>


namespace pairer {

class RadiusAvp;

class RadiusRequest;


class RadiusAvp : public virtual ::apache::thrift::TBase {
 public:

  RadiusAvp(const RadiusAvp&);
  RadiusAvp& operator=(const RadiusAvp&);
  RadiusAvp() : type(0), value() {
  }

  virtual ~RadiusAvp() throw();
  int8_t type;
  std::string value;

  void __set_type(const int8_t val);

  void __set_value(const std::string& val);

  bool operator == (const RadiusAvp & rhs) const
  {
    if (!(type == rhs.type))
      return false;
    if (!(value == rhs.value))
      return false;
    return true;
  }
  bool operator != (const RadiusAvp &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const RadiusAvp & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(RadiusAvp &a, RadiusAvp &b);

inline std::ostream& operator<<(std::ostream& out, const RadiusAvp& obj)
{
  obj.printTo(out);
  return out;
}

typedef struct _RadiusRequest__isset {
  _RadiusRequest__isset() : captureTimestamp(false), sourceId(false), avpRequestList(false), avpResponseList(false) {}
  bool captureTimestamp :1;
  bool sourceId :1;
  bool avpRequestList :1;
  bool avpResponseList :1;
} _RadiusRequest__isset;

class RadiusRequest : public virtual ::apache::thrift::TBase {
 public:

  RadiusRequest(const RadiusRequest&);
  RadiusRequest& operator=(const RadiusRequest&);
  RadiusRequest() : captureTimestamp(0), sourceId(0) {
  }

  virtual ~RadiusRequest() throw();
  int32_t captureTimestamp;
  int32_t sourceId;
  std::vector<RadiusAvp>  avpRequestList;
  std::vector<RadiusAvp>  avpResponseList;

  _RadiusRequest__isset __isset;

  void __set_captureTimestamp(const int32_t val);

  void __set_sourceId(const int32_t val);

  void __set_avpRequestList(const std::vector<RadiusAvp> & val);

  void __set_avpResponseList(const std::vector<RadiusAvp> & val);

  bool operator == (const RadiusRequest & rhs) const
  {
    if (!(captureTimestamp == rhs.captureTimestamp))
      return false;
    if (!(sourceId == rhs.sourceId))
      return false;
    if (__isset.avpRequestList != rhs.__isset.avpRequestList)
      return false;
    else if (__isset.avpRequestList && !(avpRequestList == rhs.avpRequestList))
      return false;
    if (__isset.avpResponseList != rhs.__isset.avpResponseList)
      return false;
    else if (__isset.avpResponseList && !(avpResponseList == rhs.avpResponseList))
      return false;
    return true;
  }
  bool operator != (const RadiusRequest &rhs) const {
    return !(*this == rhs);
  }

  bool operator < (const RadiusRequest & ) const;

  uint32_t read(::apache::thrift::protocol::TProtocol* iprot);
  uint32_t write(::apache::thrift::protocol::TProtocol* oprot) const;

  virtual void printTo(std::ostream& out) const;
};

void swap(RadiusRequest &a, RadiusRequest &b);

inline std::ostream& operator<<(std::ostream& out, const RadiusRequest& obj)
{
  obj.printTo(out);
  return out;
}

} // namespace

#endif
