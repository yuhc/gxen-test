#ifndef A3_CHANNEL_H_
#define A3_CHANNEL_H_
#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>
#include "a3.h"
#include "a3_session.h"
namespace a3 {
class shadow_page_table;
class context;
class page;

class channel : private boost::noncopyable {
 public:
    channel(int id);
    ~channel();
    uint64_t refresh(context* ctx, uint64_t addr);
    shadow_page_table* table() { return table_.get(); }
    const shadow_page_table* table() const { return table_.get(); }
    int id() const { return id_; }
    bool enabled() const { return enabled_; }
    uint64_t ramin_address() const { return ramin_address_; }
    page* shadow_ramin() { return shadow_ramin_.get(); }
    const page* shadow_ramin() const { return shadow_ramin_.get(); }
    void shadow(context* ctx);

 private:
    void detach(context* ctx, uint64_t addr);
    void attach(context* ctx, uint64_t addr);
    int id_;
    bool enabled_;
    uint64_t ramin_address_;
    boost::scoped_ptr<shadow_page_table> table_;
    boost::scoped_ptr<page> shadow_ramin_;
};

}  // namespace a3
#endif  // A3_CHANNEL_H_
/* vim: set sw=4 ts=4 et tw=80 : */