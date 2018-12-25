/* 
 * File:   mySPI.h
 * Author: tranthamkw
 *
 * Created on June 30, 2018, 8:03 PM
 */

#ifndef MYSPI_H
#define	MYSPI_H

#ifdef	__cplusplus
extern "C" {
#endif

    void initSPI(void);
    unsigned char writeSPI(char data_out);

#ifdef	__cplusplus
}
#endif

#endif	/* MYSPI_H */

