#ifndef __ATA_PIO_H__
#define __ATA_PIO_H__

void ata_pio_read(void *ptr, unsigned long lba, unsigned char secnum);

#endif // __ATA_PIO_H__
