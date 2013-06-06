#ifndef A3_CONTEXT_H_
#define A3_CONTEXT_H_
#include <boost/scoped_array.hpp>
#include <boost/checked_delete.hpp>
#include <boost/interprocess/smart_ptr/unique_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <boost/noncopyable.hpp>
#include "a3.h"
#include "a3_channel.h"
#include "a3_fake_channel.h"
#include "a3_session.h"
namespace a3 {
namespace barrier {
class table;
}  // namespace barrier
class poll_area;

template<class T>
struct unique_ptr {
  typedef boost::interprocess::unique_ptr< T, boost::checked_deleter<T> > type;
};

class playlist;

class context : private boost::noncopyable {
 public:
    typedef boost::unordered_multimap<uint64_t, channel*> channel_map;

    context(session* session, bool through);
    virtual ~context();
    void handle(const command& command);
    void write_bar0(const command& command);
    void write_bar1(const command& command);
    void write_bar3(const command& command);
    void read_bar0(const command& command);
    void read_bar1(const command& command);
    void read_bar3(const command& command);
    void read_barrier(uint64_t addr, const command& command);
    void write_barrier(uint64_t addr, const command& command);
    bool through() const { return through_; }
    fake_channel* bar1_channel() { return bar1_channel_.get(); }
    const fake_channel* bar1_channel() const { return bar1_channel_.get(); }
    fake_channel* bar3_channel() { return bar3_channel_.get(); }
    const fake_channel* bar3_channel() const { return bar3_channel_.get(); }
    channel* channels(int id) { return channels_[id].get(); }
    const channel* channels(int id) const { return channels_[id].get(); }
    barrier::table* barrier() { return barrier_.get(); }
    const barrier::table* barrier() const { return barrier_.get(); }
    channel_map* ramin_channel_map() { return &ramin_channel_map_; }
    const channel_map* ramin_channel_map() const { return &ramin_channel_map_; }
    playlist* fifo_playlist() { return fifo_playlist_.get(); }
    const playlist* fifo_playlist() const { return fifo_playlist_.get(); }
    uint64_t vram_size() const { return A3_2G; }
    uint64_t get_address_shift() const {
        return id() * vram_size();
    }
    uint64_t get_phys_address(uint64_t virt) const {
        return virt + get_address_shift();
    }
    uint64_t get_virt_address(uint64_t phys) const {
        return phys - get_address_shift();
    }
    uint32_t get_phys_channel_id(uint32_t virt) const {
        return virt + id() * A3_DOMAIN_CHANNELS;
    }
    uint32_t get_virt_channel_id(uint32_t phys) const {
        return phys - id() * A3_DOMAIN_CHANNELS;
    }
    uint32_t id() const { return id_; }
    int domid() const { return domid_; }
    uint64_t poll_area() const { return poll_area_; }
    bool flush(uint64_t pd, bool bar = false);
    command* buffer() { return session_->buffer(); }

 private:
    void initialize(int domid);
    void fifo_playlist_update(uint32_t reg_addr, uint32_t cmd);
    void flush_tlb(uint32_t vspace, uint32_t trigger);
    bool in_poll_area(uint64_t offset) const {
        return poll_area() <= offset && offset < poll_area() + (A3_DOMAIN_CHANNELS * 0x1000);
    }
    uint32_t decode_to_virt_ramin(uint32_t value);
    uint32_t encode_to_shadow_ramin(uint32_t value);
    bool shadow_ramin_to_phys(uint64_t shadow, uint64_t* phys);

    session* session_;
    bool through_;
    bool initialized_;
    int domid_;
    uint32_t id_;  // virtualized GPU id
    unique_ptr<fake_channel>::type bar1_channel_;
    unique_ptr<fake_channel>::type bar3_channel_;
    boost::array<unique_ptr<channel>::type, A3_DOMAIN_CHANNELS> channels_;
    unique_ptr<barrier::table>::type barrier_;
    unique_ptr<playlist>::type fifo_playlist_;

    uint64_t poll_area_;

    boost::scoped_array<uint32_t> reg_;
    channel_map ramin_channel_map_;
};

}  // namespace a3
#endif  // A3_CONTEXT_H_
/* vim: set sw=4 ts=4 et tw=80 : */
