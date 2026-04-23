#ifndef __GAME_SYS__
#define __GAME_SYS__

typedef struct event_s event_t;
typedef void (*EventCallback)(
    event_t    *event,
    void*      user_data
);

typedef struct {
    EventType      event;
    EventCallback  cb;
    uint64_t       uid;
    void*          user_data;
} event_sub_t;

typedef struct {
    event_sub_t* subs;
    hash_map_t   scheduled;
    int          count, cap;
} event_bus_t;

struct event_s{
  game_object_uid_i gouid;
  EventType         type;
  EventStatus       status;
  int               max, calls;
  void*             data;
  uint64_t          iuid;
  TimeFrame         timing;
  int               scheduled;
  param_t           payload;
};

event_bus_t* InitEventBus(int cap);
void EventBusStep(event_bus_t* bus);
event_sub_t* EventSubscribe(event_bus_t* bus, EventType event, EventCallback cb, void* u_data);
void EventEmit(event_bus_t* bus, event_t*);
void EventRemove(event_bus_t* bus, uint64_t id);
game_object_uid_i EventSchedule(event_bus_t* bus, event_t* e);
#endif
