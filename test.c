#include "test.h"
#include "const.h"

void init_blk(volatile virtio_t *vio) {
  volatile virtio_blk_config_t *config = (volatile virtio_blk_config_t *)&vio->config;
  printf("  capacity:      %lu MB\n", config->capacity * config->blk_size / 1024 / 1024);
  printf("  block size:    %u Bytes\n", config->blk_size);
  vio->status |= VIRTIO_DRIVER_OK;
  wmb();
  // init virt queue
  vio->guest_page_size = PGSIZE;
  vio->queue_sel = 0; // select queue 0
  wmb();
  if (vio->queue_pfn != 0) {
    puts("???");
    for (;;);
  }
  static virtq_desc_t desc[2];
  if (((uintptr_t)&desc) & 0xf != 0) {
    puts("GG");
    for (;;);
  }
  extern virtq_t virtq;
  vio->queue_num = 2;
  vio->queue_align = 16;
  vio->queue_pfn = __PA(&virtq) >> PGSHIFT;
  wmb();
  /*vio->queue_desc_low = __PA(&desc) & 0xffffffff;
  vio->queue_desc_high = (__PA(&desc) >> 32) & 0xffffffff;
  vio->queue_avail_low = __PA(&avail) & 0xffffffff;
  vio->queue_avail_high = (__PA(&avail) >> 32) & 0xffffffff;
  vio->queue_used_low = __PA(&used) & 0xffffffff;
  vio->queue_used_high = (__PA(&used) >> 32) & 0xffffffff;
  wmb();*/
  vio->queue_ready = 1;
  wmb();
  puts("  Initialized queue.");
  puts("  test: writing...");
  static virtio_blk_req_t req;
  req.type = VIRTIO_BLK_T_OUT;
  req.reserved = 0;
  req.sector = 1;
  memset(req.data, 0x66, sizeof(req.data));
  req.status = 0;
  virtq.desc[0].addr = __PA(&req);
  virtq.desc[0].len = sizeof(req);
  virtq.desc[0].flags = VIRTQ_DESC_F_WRITE;
  virtq.avail.ring[virtq.avail.idx & 1] = 0;
  wmb();
  ++virtq.avail.idx;
  wmb();
  vio->queue_notify = 0;
  wmb();
}

const char *devid_to_str(uint32_t devid) {
  if (devid == VIRTIO_DEVID_NET) {
    return "Network Device";
  }
  if (devid == VIRTIO_DEVID_BLK) {
    return "Block Device";
  }
  return "Unknown";
}

void test(uintptr_t fdt) {
  for (int i = 1; i < 9; ++i) {
    volatile virtio_t *vio = __VA(0x10000000 + i * 0x1000);
    if (vio->magic == 0x74726976 && vio->version <= 1 && vio->device_id != VIRTIO_DEVID_NONE) {
      printf("%p is virtio device, version %d!\n", vio, vio->version);
    } else {
      continue;
    }
    printf("  version:       %u\n", vio->version);
    printf("  device_id:     %s\n", devid_to_str(vio->device_id));
    printf("  vendor_id:     0x%08x\n", vio->vendor_id);
    printf("  dev_features:  0x%08x\n", vio->device_features);
    printf("  queue_num_max: %u\n", vio->queue_num_max);
    printf("  status:        0x%08x\n", vio->status);
    printf("  config_gen:    %u\n", vio->config_generation);
    puts("  initializing...");
    vio->status = 0;
    wmb();
    vio->status |= VIRTIO_ACK;
    wmb();
    vio->status |= VIRTIO_DRIVER;
    wmb();
    uint32_t features = vio->device_features;
    printf("  dev_features:  0x%08x\n", features);
    vio->device_features = features;
    wmb();
    if (vio->device_id == VIRTIO_DEVID_BLK) {
      init_blk(vio);
    } /*else if (vio->device_id == VIRTIO_DEVID_NET) {
      init_net(vio); // TODO
    }*/ else {
      puts("  Don't know how to initialize.");
      vio->status |= VIRTIO_FAILED;
      wmb();
    }
  }
}
