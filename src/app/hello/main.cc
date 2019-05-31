#include <base/component.h>

void Component::construct(Genode::Env &env)
{
  Genode::Ram_dataspace_capability cap = env.pd().alloc(100);
  char *bar = env.rm().attach(cap);
  Genode::log("Hello World from hello! ",*bar);
  Genode::log("second msg!");
}
