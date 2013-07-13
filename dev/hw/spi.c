#include <list.h>
#include <dev/hw/spi.h>
#include <kernel/class.h>
#include <kernel/fault.h>
#include <kernel/system.h>
#include <mm/mm.h>

static void spi_dtor(struct obj *o);

struct obj_type spi_type_s  = {
    .offset = offset_of(struct spi, obj),
    .dtor = &spi_dtor,
};

struct class spi_class = INIT_CLASS(spi_class, "spi", &spi_type_s);

static void spi_dtor(struct obj *o) {
    struct spi *s;
    struct spi_ops *ops;

    assert_type(o, &spi_type_s);
    s = (struct spi *) to_spi(o);
    ops = (struct spi_ops *)o->ops;
    ops->deinit(s);

    if (s->priv) {
        kfree(s->priv);
    }
}
