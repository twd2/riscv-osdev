#ifndef _TEST_H_
#define _TEST_H_

#include "defs.h"

#define VIRTIO_DEVID_NONE 0
#define VIRTIO_DEVID_BLK 2
#define VIRTIO_DEVID_NET 1

#define VIRTIO_ACK 1
#define VIRTIO_DRIVER 2
#define VIRTIO_DRIVER_OK 4
#define VIRTIO_FAILED 128

typedef struct virtio {
  uint32_t magic; // 00
  uint32_t version; // 04
  uint32_t device_id; // 08
  uint32_t vendor_id; // 0c
  uint32_t device_features; // 10
  uint32_t device_features_sel; // 14
  uint32_t padding0; // 18
  uint32_t padding1; // 1c
  uint32_t driver_features; // 20
  uint32_t driver_features_sel; // 24
  uint32_t guest_page_size; // 28
  uint32_t padding3; // 2c
  uint32_t queue_sel; // 30
  uint32_t queue_num_max; // 34
  uint32_t queue_num; // 38
  uint32_t queue_align; // 3c
  uint32_t queue_pfn; // 40
  uint32_t queue_ready; // 44
  uint32_t padding6; // 48
  uint32_t padding7; // 4c
  uint32_t queue_notify; // 50
  uint32_t padding8; // 54
  uint32_t padding9; // 58
  uint32_t padding10; // 5c
  uint32_t interrupt_status; // 60
  uint32_t interrput_ack; // 64
  uint32_t padding11; // 68
  uint32_t padding12; // 6c
  uint32_t status; // 70
  uint32_t padding13; // 74
  uint32_t padding14; // 78
  uint32_t padding15; // 7c
  uint32_t queue_desc_low; // 80
  uint32_t queue_desc_high; // 84
  uint32_t padding16; // 88
  uint32_t padding17; // 8c
  uint32_t queue_avail_low; // 90
  uint32_t queue_avail_high; // 94
  uint32_t padding18; // 98
  uint32_t padding19; // 9c
  uint32_t queue_used_low; // a0
  uint32_t queue_used_high; // a4
  char padding[0xfc - 0xa8]; // a8
  uint32_t config_generation; // fc
  char config[0]; // 0x100
} virtio_t;

/* This marks a buffer as continuing via the next field. */
#define VIRTQ_DESC_F_NEXT 1
/* This marks a buffer as write-only (otherwise read-only). */
#define VIRTQ_DESC_F_WRITE 2
/* This means the buffer contains a list of buffer descriptors. */
#define VIRTQ_DESC_F_INDIRECT 4
/* The device uses this in used->flags to advise the driver: don't kick me
* when you add a buffer. It's unreliable, so it's simply an
* optimization. */
#define VIRTQ_USED_F_NO_NOTIFY 1
/* The driver uses this in avail->flags to advise the device: don't
* interrupt me when you consume a buffer. It's unreliable, so it's
* simply an optimization. */
#define VIRTQ_AVAIL_F_NO_INTERRUPT 1
/* Support for indirect descriptors */
#define VIRTIO_F_INDIRECT_DESC 28
/* Support for avail_event and used_event fields */
#define VIRTIO_F_EVENT_IDX 29
/* Arbitrary descriptor layouts. */
#define VIRTIO_F_ANY_LAYOUT 27

typedef struct virtq_desc {
  /* Address (guest-physical). */
  uint64_t addr;
  /* Length. */
  uint32_t len;
  /* The flags as indicated above. */
  uint16_t flags;
  /* Next field if flags & NEXT */
  uint16_t next;
} virtq_desc_t;

typedef struct virtq_avail {
  uint16_t flags;
  uint16_t idx;
  uint16_t ring[2];
  /* Only if VIRTIO_F_EVENT_IDX: uint16_t used_event; */
} virtq_avail_t;

/* le32 is used here for ids for padding reasons. */
typedef struct virtq_used_elem {
  /* Index of start of used descriptor chain. */
  uint32_t id;
  /* Total length of the descriptor chain which was written to. */
  uint32_t len;
} virtq_used_elem_t;

typedef struct virtq_used {
  uint16_t flags;
  uint16_t idx;
  virtq_used_elem_t ring[2];
  /* Only if VIRTIO_F_EVENT_IDX: le16 avail_event; */
} virtq_used_t;

typedef struct virtq {
  // The actual descriptors (16 bytes each)
  struct virtq_desc desc[2];
  // A ring of available descriptor heads with free-running index.
  virtq_avail_t avail;
  // Padding to the next Queue Align boundary.
  uint8_t pad[8];
  // A ring of used descriptor heads with free-running index.
  virtq_used_t used;
} virtq_t;

#define VIRTIO_BLK_T_IN 0
#define VIRTIO_BLK_T_OUT 1
#define VIRTIO_BLK_T_FLUSH 4

#define VIRTIO_BLK_S_OK 0
#define VIRTIO_BLK_S_IOERR 1
#define VIRTIO_BLK_S_UNSUPP 2

typedef struct virtio_blk_config {
  uint64_t capacity;
  uint32_t size_max;
  uint32_t seg_max;
  struct virtio_blk_geometry {
    uint16_t cylinders;
    uint8_t heads;
    uint8_t sectors;
  } geometry;
  uint32_t blk_size;
  struct virtio_blk_topology {
    // # of logical blocks per physical block (log2)
    uint8_t physical_block_exp;
    // offset of first aligned logical block
    uint8_t alignment_offset;
    // suggested minimum I/O size in blocks
    uint16_t min_io_size;
    // optimal (suggested maximum) I/O size in blocks
    uint32_t opt_io_size;
  } topology;
  uint8_t writeback;
} virtio_blk_config_t;

typedef struct virtio_blk_req {
  uint32_t type;
  uint32_t reserved;
  uint64_t sector;
  uint8_t data[512];
  uint8_t status;
} virtio_blk_req_t;

void test(uintptr_t fdt);

#endif // _TEST_H_
