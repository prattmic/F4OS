#include <dev/hw/i2c.h>
#include <kernel/class.h>
#include <mm/mm.h>

static void i2c_dtor(struct obj *o);

struct obj_type i2c_type_s  = {
    .offset = offset_of(struct i2c, obj),
    .dtor = &i2c_dtor,
};

struct class i2c_class = INIT_CLASS(i2c_class, "i2c", &i2c_type_s);

static void i2c_dtor(struct obj *o) {
    struct i2c *s;
    struct i2c_ops *ops;

    assert_type(o, &i2c_type_s);
    s = (struct i2c *) to_i2c(o);
    ops = (struct i2c_ops *)o->ops;
    ops->deinit(s);

    if (s->priv) {
        kfree(s->priv);
    }
}
