/* 
 * File:   gpib.h
 * Author: tranthamkw
 *
 * Created on July 15, 2018, 10:20 PM
 */

#ifndef GPIB_H
#define	GPIB_H

#ifdef	__cplusplus
extern "C" {
#endif

#ifndef GPIB_H_
#define GPIB_H_


#define TIMEOUT 100000




void gpib_init(void);
void sendgpib (void);
int readgpib(void);
void gpib_ren(unsigned char state);
void gpib_ifc(void);

#endif



#ifdef	__cplusplus
}
#endif

#endif	/* GPIB_H */

