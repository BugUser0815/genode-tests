#include <base/component.h>
#include <base/log.h>

#include <util/string.h>

#include <base/child.h>
#include <init/child_policy.h>

#include <rom_session/connection.h>

#include <loader_session/connection.h>

#include <rm_session/connection.h>

/* custom services */
#include <root/component.h>

using namespace Genode;

namespace Custom {
  class Rm_session_component;
  class Rm_session_factory;
  class Region_map_component;
  class Pd_session_component;
  class Pd_session_factory;
  class Cpu_session_component;
  class Cpu_session_factory;
  struct Resources;
}

namespace Example {
  struct Main;
}

class Custom::Region_map_component : public Genode::Rpc_object<Genode::Region_map>, public List<Region_map_component>::Element
{
  Genode::Rpc_entrypoint &_ep;
  Genode::Env &_env;
  Genode::Region_map_client  _parent_region_map;

  public:

  Region_map_component(Genode::Rpc_entrypoint &ep, Genode::Env &env, Genode::Capability<Genode::Region_map> region_map_cap)
  :
    _ep(ep),
    _env(env),
    _parent_region_map{region_map_cap}
  {
    _ep.manage(this);
  }

  ~Region_map_component() {
    _ep.dissolve(this);
  }

  Genode::Region_map::Local_addr attach(Genode::Dataspace_capability cap, Genode::size_t foo, Genode::off_t bar, bool foobar, Genode::Region_map::Local_addr addr, bool barfoo)
  {
    Genode::log("Rm::\033[33m", __func__, "\033[0m");
    /*Genode::log("size: ",foo);
    int *baum = _env.rm().attach(cap);
    for(size_t i=0; i<foo; i++)
    {
      int *tmp=baum+i;
      Genode::log(*tmp);
    }
    _env.rm().detach(baum);*/
    return _parent_region_map.attach(cap, foo, bar, foobar, addr, barfoo);
  }

  void detach(Genode::Region_map::Local_addr addr)
  {
    Genode::log("Rm::\033[33m", __func__, "\033[0m");
    _parent_region_map.detach(addr);
  }

  void fault_handler(Genode::Signal_context_capability cap)
  {
    Genode::log("Rm::\033[33m", __func__, "\033[0m");
    _parent_region_map.fault_handler(cap);
  }

  Genode::Region_map::State state()
  {
    Genode::log("Rm::\033[33m", __func__, "\033[0m");
    return _parent_region_map.state();
  }

  Genode::Dataspace_capability dataspace()
  {
    Genode::log("Rm::\033[33m", __func__, "\033[0m");
    return _parent_region_map.dataspace();
  }
};

class Custom::Rm_session_component : public Genode::Rpc_object<Genode::Rm_session>
{
  Genode::Rpc_entrypoint &_ep;
  Genode::Env &_env;
  Genode::Rm_connection _parent_rm;
  Genode::Heap _heap { _env.ram(), _env.rm() };
  List<Custom::Region_map_component> _region_maps      { };

  public:

  Rm_session_component(Genode::Rpc_entrypoint &ep, Genode::Env &env, const char *label)
  :
    _ep(ep),
    _env(env),
    _parent_rm(env)
  {
    _ep.manage(this);
  }

  ~Rm_session_component() {
    _ep.dissolve(this);
  }

  Capability<Region_map> create(size_t size)
  {
    Genode::log("Rm::\033[33m", __func__, "\033[0m");
    Capability<Region_map> cap = _parent_rm.create(size);
    return cap;
  }

  void destroy(Capability<Region_map> cap)
  {
    _parent_rm.destroy(cap);
  }
  
};

class Custom::Pd_session_component : public Genode::Rpc_object<Genode::Pd_session>
{
  Genode::Rpc_entrypoint &_ep;
  Genode::Env &_env;
  Genode::Pd_connection _parent_pd;
  Genode::Heap _heap { _env.ram(), _env.rm() };

  public:

  Pd_session_component(Genode::Rpc_entrypoint &ep, Genode::Env &env, const char *label)
  :
    _ep(ep),
    _env(env),
    _parent_pd(env, label)
  {
    _ep.manage(this);
  }

  ~Pd_session_component() {
    _ep.dissolve(this);
  }

  Ram_dataspace_capability alloc(Genode::size_t size, Genode::Cache_attribute attr)
  {
    Genode::log("Pd::\033[33m", __func__, "\033[0m");
    Ram_dataspace_capability cap = _parent_pd.alloc(size, attr);
    return cap;
  }

  void free(Ram_dataspace_capability ds)
  {
    Genode::log("Pd::\033[33m", __func__, "\033[0m");
    _parent_pd.free(ds);
  }

  size_t dataspace_size(Ram_dataspace_capability ds) const
  {
    Genode::log("Pd::\033[33m", __func__, "\033[0m");
    return _parent_pd.dataspace_size(ds);
  }

  void assign_parent(Capability<Parent> parent)
  {
    Genode::log("Pd::\033[33m", __func__, "\033[0m");
    _parent_pd.assign_parent(parent);
  }

  bool assign_pci(addr_t pci_config_memory_address, uint16_t bdf)
  {
    Genode::log("Pd::\033[33m", __func__, "\033[0m");
    return _parent_pd.assign_pci(pci_config_memory_address, bdf);
  }

  void map(addr_t virt, addr_t size)
  {
    Genode::log("Pd::\033[33m", __func__, "\033[0m");
    _parent_pd.map(virt, size);
  }

  Signal_source_capability alloc_signal_source()
  {
    Genode::log("Pd::\033[33m", __func__, "\033[0m");
    return _parent_pd.alloc_signal_source();
  }

  void free_signal_source(Signal_source_capability cap)
  {
    Genode::log("Pd::\033[33m", __func__, "\033[0m");
    _parent_pd.free_signal_source(cap);
  }

  Capability<Signal_context> alloc_context(Signal_source_capability source, long unsigned int imprint)
  {
    Genode::log("Pd::\033[33m", __func__, "\033[0m");
    return _parent_pd.alloc_context(source, imprint);
  }

  void free_context(Capability<Signal_context> cap)
  {
    Genode::log("Pd::\033[33m", __func__, "\033[0m");
    _parent_pd.free_context(cap);
  }

  void submit(Capability<Signal_context> context, unsigned int cnt)
  {
    Genode::log("Pd::\033[33m", __func__, "\033[0m");
    _parent_pd.submit(context, cnt);
  }

  Native_capability alloc_rpc_cap(Native_capability ep)
  {
    Genode::log("Pd::\033[33m", __func__, "\033[0m");
    return _parent_pd.alloc_rpc_cap(ep);
  }
  
  void free_rpc_cap(Native_capability cap)
  {
    Genode::log("Pd::\033[33m", __func__, "\033[0m");
    _parent_pd.free_rpc_cap(cap);
  }

  Capability<Region_map> address_space()
  {
    Genode::log("Pd::\033[33m", __func__, "\033[0m");
    Custom::Region_map_component *rm = new (_heap) Custom::Region_map_component(_ep,_env,_parent_pd.address_space());
    Capability<Region_map> cap = rm->Rpc_object<Genode::Region_map>::cap();
    return cap;
  }

  Capability<Region_map> stack_area()
  {
    Genode::log("Pd::\033[33m", __func__, "\033[0m");
    Capability<Region_map> cap = _parent_pd.stack_area();
    return cap;
  }

  Capability<Region_map> linker_area()
  {
    Genode::log("Pd::\033[33m", __func__, "\033[0m");
    return _parent_pd.linker_area();
  }

  void ref_account(Capability<Pd_session> pd_cap)
  {
    Genode::log("Pd::\033[33m", __func__, "\033[0m");
    _parent_pd.ref_account(pd_cap);
  }

  void transfer_quota(Capability<Pd_session> to, Cap_quota amount)
  {
    Genode::log("Pd::\033[33m", __func__, "\033[0m");
    _parent_pd.transfer_quota(to, amount);
  }

  Cap_quota cap_quota() const
  {
    Genode::log("Pd::\033[33m", __func__, "\033[0m");
    return _parent_pd.cap_quota();
  }

  Cap_quota used_caps() const
  {
    Genode::log("Pd::\033[33m", __func__, "\033[0m");
    return _parent_pd.used_caps();
  }

  void transfer_quota(Capability<Pd_session> to, Ram_quota amount)
  {
    Genode::log("Pd::\033[33m", __func__, "\033[0m");
    _parent_pd.transfer_quota(to, amount);
  }

  Ram_quota ram_quota() const
  {
    Genode::log("Pd::\033[33m", __func__, "\033[0m");
    return _parent_pd.ram_quota();
  }

  Ram_quota used_ram() const
  {
    Genode::log("Pd::\033[33m", __func__, "\033[0m");
    return _parent_pd.used_ram();
  }

  Capability<Native_pd> native_pd()
  {
    Genode::log("Pd::\033[33m", __func__, "\033[0m");
    return _parent_pd.native_pd();
  }

  Pd_session_capability parent_pd_cap()
  {
    return _parent_pd.cap();
  }
  
};

struct Resources
{
  Custom::Pd_session_component *_cps;
} static resources;

class Custom::Cpu_session_component : public Genode::Rpc_object<Genode::Cpu_session>
{
  Genode::Rpc_entrypoint &_ep;
  Genode::Env &_env;
  Genode::Cpu_connection _parent_cpu;

  public:

  Cpu_session_component(Genode::Rpc_entrypoint &ep, Genode::Env &env, const char *label)
  :
    _ep(ep),
    _env(env),
    _parent_cpu(env, label)
  {
    _ep.manage(this);
  }

  ~Cpu_session_component() {
    _ep.dissolve(this);
  }

  Genode::Thread_capability create_thread(Genode::Capability<Genode::Pd_session> pd_cap, const Name& name, Genode::Affinity::Location location, Genode::Cpu_session::Weight weight, Genode::addr_t addr)
  {
    Genode::log("Cpu::\033[33m", __func__, "\033[0m");
    //cpu thread is created within our custom pd
    return _parent_cpu.create_thread(resources._cps->parent_pd_cap(), name, location, weight, addr);
  }

  void kill_thread(Genode::Thread_capability thread_cap)
  {
    Genode::log("Cpu::\033[33m", __func__, "\033[0m");
    _parent_cpu.kill_thread(thread_cap);
  }

  void exception_sigh(Genode::Signal_context_capability sig_cap)
  {
    Genode::log("Cpu::\033[33m", __func__, "\033[0m");
    _parent_cpu.exception_sigh(sig_cap);
  }

  Genode::Affinity::Space affinity_space() const
  {
    Genode::log("Cpu::\033[33m", __func__, "\033[0m");
    return _parent_cpu.affinity_space();
  }

  Genode::Dataspace_capability trace_control()
  {
    Genode::log("Cpu::\033[33m", __func__, "\033[0m");
    return _parent_cpu.trace_control();
  }

  int ref_account(Genode::Cpu_session_capability cpu_cap)
  {
    Genode::log("Cpu::\033[33m", __func__, "\033[0m");
    return _parent_cpu.ref_account(cpu_cap);
  }

  int transfer_quota(Genode::Cpu_session_capability cpu_cap, Genode::size_t size)
  {
    Genode::log("Cpu::\033[33m", __func__, "\033[0m");
    return _parent_cpu.transfer_quota(cpu_cap, size);
  }

  Genode::Cpu_session::Quota quota()
  {
    Genode::log("Cpu::\033[33m", __func__, "\033[0m");
    return _parent_cpu.quota();
  }

  Genode::Capability<Genode::Cpu_session::Native_cpu> native_cpu()
  {
    Genode::log("Cpu::\033[33m", __func__, "\033[0m");
    return _parent_cpu.native_cpu();
  }

  void deploy_queue(Genode::Dataspace_capability ds_cap)
  {
    Genode::log("Cpu::\033[33m", __func__, "\033[0m");
    return _parent_cpu.deploy_queue(ds_cap);
  }

  void rq(Genode::Dataspace_capability ds_cap)
  {
    Genode::log("Cpu::\033[33m", __func__, "\033[0m");
    return _parent_cpu.rq(ds_cap);
  }

  void dead(Genode::Dataspace_capability ds_cap)
  {
    Genode::log("Cpu::\033[33m", __func__, "\033[0m");
    return _parent_cpu.dead(ds_cap);
  }

  void killed()
  {
    Genode::log("Cpu::\033[33m", __func__, "\033[0m");
    return _parent_cpu.killed();
  }
};

class Custom::Pd_session_factory : public Genode::Local_service<Custom::Pd_session_component>::Factory
{
  Genode::Allocator &_alloc;
  Genode::Rpc_entrypoint &_ep;
  Genode::Env &_env;

  public:

  Pd_session_factory(Genode::Allocator &alloc, Genode::Rpc_entrypoint &ep, Genode::Env &env)
  :
    _alloc(alloc),
    _ep(ep),
    _env(env)
  { }

  Custom::Pd_session_component &create(Args const &priargs, Genode::Affinity) override
  {
    Genode::log("Pd::\033[33m", __func__, "\033[0m");
    resources._cps=new (_alloc) Custom::Pd_session_component(_ep, _env, "hello");
    return *resources._cps;
  }

  void upgrade(Custom::Pd_session_component &, Args const &) override { }

  void destroy(Custom::Pd_session_component &session) override
  {
    Genode::destroy(_alloc, &session);
  }
};

class Custom::Cpu_session_factory : public Genode::Local_service<Custom::Cpu_session_component>::Factory
{
  Genode::Allocator &_alloc;
  Genode::Rpc_entrypoint &_ep;
  Genode::Env &_env;

  public:

  Cpu_session_factory(Genode::Allocator &alloc, Genode::Rpc_entrypoint &ep, Genode::Env &env)
  :
    _alloc(alloc),
    _ep(ep),
    _env(env)
  { }

  Custom::Cpu_session_component &create(Args const &priargs, Genode::Affinity) override
  {
    return *new (_alloc) Custom::Cpu_session_component(_ep, _env, "hello");
  }

  void upgrade(Custom::Cpu_session_component &, Args const &) override { }

  void destroy(Custom::Cpu_session_component &session) override
  {
    Genode::destroy(_alloc, &session);
  }
};


class MyChild : public Child_policy {
public:
  typedef Genode::Registered<Genode::Parent_service> Parent_service;
  typedef Genode::Registry<Parent_service> Parent_services;
private:
  Env &_env;
  Cap_quota const _cap_quota {50};
  Ram_quota const _ram_quota {1 * 1024 * 1024};

  Parent_services &_parent_services;
  Genode::Local_service<Custom::Pd_session_component> &_ps;
  Genode::Local_service<Custom::Cpu_session_component> &_cs;

  Child _child;

  template <typename T>
  static Genode::Service *_find_service(Genode::Registry<T> &services,
					Genode::Service::Name const &name)
  {
    Genode::Service *service = nullptr;
    services.for_each([&] (T &s) {
			if (!service && (s.name() == name))
			  service = &s; });
    return service;
  }

public:
  MyChild(Env &env, Parent_services &parent_services, Genode::Local_service<Custom::Pd_session_component> &ps, Genode::Local_service<Custom::Cpu_session_component> &cs)
    :
    _env(env), _parent_services(parent_services), _ps(ps), _cs(cs), _child(_env.rm(), _env.ep().rpc_ep(), *this)
  {}
  ~MyChild() { };

  Name name() const override { return "hello"; };

  Pd_session           &ref_pd()           override { return _env.pd(); }
  Pd_session_capability ref_pd_cap() const override { return _env.pd_session_cap(); }

  void init(Pd_session &pd, Pd_session_capability pd_cap) override
  {
    Genode::log("MyChild init");
    //env is our ref account
    pd.ref_account(ref_pd_cap());
    Genode::log("MyChild ref_account");
    resources._cps->parent_pd_cap();
    //tell environment to transfer quota to custom pd session's connection with env, alias _parent_pd
    ref_pd().transfer_quota(resources._cps->parent_pd_cap(), _cap_quota);
    ref_pd().transfer_quota(resources._cps->parent_pd_cap(), _ram_quota);
  }

  Genode::Child_policy::Route
  resolve_session_request(Genode::Service::Name const &service_name,
			  Genode::Session_label const &label) override
  {
    auto route = [&] (Genode::Service &service) {
		   return Genode::Child_policy::Route { .service = service,
							 .label   = label,
							 .diag    = Genode::Session::Diag() }; };

    Genode::Service *service = nullptr;
    service = _find_service(_parent_services, service_name);
    if (!strcmp("PD", service_name.string())) {
      Genode::log("--> ", service_name.string(), " session intercepter --> use our own ", service_name.string(), " session");
      service = &_ps;
    }
    if (!strcmp("CPU", service_name.string())) {
      Genode::log("--> ", service_name.string(), " session intercepter --> use our own ", service_name.string(), " session");
      service = &_cs;
    }
    return route(*service);
  }
};

class Custom::Rm_session_factory : public Genode::Local_service<Custom::Rm_session_component>::Factory
{
  Genode::Allocator &_alloc;
  Genode::Rpc_entrypoint &_ep;
  Genode::Env &_env;

  public:

  Rm_session_factory(Genode::Allocator &alloc, Genode::Rpc_entrypoint &ep, Genode::Env &env)
  :
    _alloc(alloc),
    _ep(ep),
    _env(env)
  { }

  Custom::Rm_session_component &create(Args const &priargs, Genode::Affinity) override
  {
    Genode::log("Pd::\033[33m", __func__, "\033[0m");
    Custom::Rm_session_component *foo = new (_alloc) Custom::Rm_session_component(_ep, _env, "hello");
    return *foo;
  }

  void upgrade(Custom::Rm_session_component &, Args const &) override { }

  void destroy(Custom::Rm_session_component &session) override
  {
    Genode::destroy(_alloc, &session);
  }
};

struct Example::Main
{
  Genode::Env &_env;

  Genode::Heap _heap { _env.ram(), _env.rm() };

  MyChild::Parent_services _parent_services { };

  Main(Genode::Env &env) : _env(env)
  {
    const char *names[] = { "RM", "PD", "CPU", "IO_MEM", "IO_PORT", "IRQ", "ROM", "LOG", 0 };

    for (unsigned i = 0; names[i]; i++) {
      new (_heap) MyChild::Parent_service(_parent_services, env, names[i]);
    }

    /* custom service */
    // create new entrypoint
    Genode::Entrypoint *_ep = new (_heap) Genode::Entrypoint(_env, 16 * 1024, "custom ep");
    Custom::Pd_session_factory _cpf { _heap, _ep->rpc_ep(), _env };
    Genode::Local_service<Custom::Pd_session_component> cps { _cpf };
    Custom::Cpu_session_factory _ccf { _heap, _ep->rpc_ep(), _env };
    Genode::Local_service<Custom::Cpu_session_component> ccs { _ccf };

    /******************/

    new (_heap) MyChild(_env, _parent_services, cps, ccs);
  }
};

void Component::construct(Genode::Env &env)
{
  Genode::log("Hello World from child!");

  static Example::Main main(env);

  /* The component does not exit after the construct function returns. Instead, it be-
   * comes ready to respond to requests or signals originating from other components. The
   * example above does not interact with other components though.
   *
   * Genode Foundations p. 30
   */
}
