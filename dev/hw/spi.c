#include <dev/hw/spi.h>
#include <kernel/class.h>
#include <mm/mm.h>

static void spi_dtor(struct obj *o);

struct obj_type spi_type_s  = {
    .offset = offset_of(struct spi, obj),
    .dtor = &spi_dtor,
};

struct class spi_class = INIT_CLASS(spi_class, "spi", &spi_type_s);

static void spi_dtor(struct obj *o) {
    struct spi *spi;
    struct spi_ops *ops;

    assert_type(o, &spi_type_s);
    spi = to_spi(o);
    ops = (struct spi_ops *)o->ops;
    ops->deinit(spi);

    if (spi->priv) {
        kfree(spi->priv);
    }
}
