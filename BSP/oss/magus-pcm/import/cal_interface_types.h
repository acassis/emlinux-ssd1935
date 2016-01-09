/****************************************************************************/
/*     THIS HEADER INCLUDES DATA TYPES USED IN CAL INTERFACE                */
/*     ----------------------------------------------------------------     */
/****************************************************************************/
#ifndef CAL_INTERFACE_H
#define CAL_INTERFACE_H

 	typedef char            S8;
	typedef unsigned char   U8;
	typedef short           S16;
	typedef unsigned short  U16;
	typedef long            S32;
	typedef unsigned long   U32;


/************* Version Struct ************/
/**
* @struct cal_version_data_struct
* @brief codec version structure
*
* structure that is uded while getting version of a media codec
*/

	typedef struct 
	{					
	   /** @var <magor_u16 MSB of codec version> */	
	   U16 magor_u16;
	   /** @var <minor_u16 LSM of codec version> */	
	   U16 minor_u16;
	   /** @var <build_u16 build number of codec> */	
	   U16 build_u16;
	   /** @var <date_u16 date of codec release> */	
	   U16 date_u16;
	   /** @var <year_u16 year of codec release> */	
	   U16 year_u16;		
	} cal_version_data_struct;


#endif /* #ifndef CAL_INTERFACE_H*/

/* nothing beyond this */

