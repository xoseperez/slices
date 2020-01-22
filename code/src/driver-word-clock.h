/*

WORD CLOCK DRIVER

Copyright (C) 2017-2020-2020 by Xose Pérez <xose dot perez at gmail dot com>

*/

#if ENABLE_DRIVER_WORDCLOCK

// CATALAN

#define CAT_ES          {0, 0xC000}
#define CAT_SON         {0, 0x7000}
#define CAT_VORA        {0, 0x00F0}

#define CAT_UN_Q        {1, 0xC000}
#define CAT_DOS_Q       {1, 0x1C00} // {1, 0x3800}
#define CAT_TRES_Q      {1, 0x03C0} // {1, 0x0780}
#define CAT_MIG         {1, 0x0007}

#define CAT_QUART       {2, 0xF800}
#define CAT_QUARTS      {2, 0xFC00}
#define CAT_I           {2, 0x0100}
#define CAT_I_MIG       {2, 0x0107}
#define CAT_MENYS       {2, 0x00F8}

#define CAT_CINC_Q      {3, 0xF000}
#define CAT_BEN_Q       {3, 0x0700}
#define CAT_TOCAT       {3, 0x003E}
#define CAT_TOCATS      {3, 0x003F}

#define CAT_LES         {4, 0xE000}
#define CAT_DE          {4, 0x1800}
#define CAT_LA          {4, 0x0600}
#define CAT_DUES        {4, 0x00F0}
#define CAT_SIS         {4, 0x001C}
#define CAT_SET         {4, 0x0007}

#define CAT_CINC        {5, 0xF000}
#define CAT_VUIT        {5, 0x00F0}
#define CAT_UNA         {5, 0x0007}
#define CAT_D_UNA       {5, 0x000F}

#define CAT_DOS         {6, 0xE000}

#define CAT_QUATRE      {7, 0xFC00}
#define CAT_TRES        {7, 0x1E00}

#define CAT_ONZE        {9, 0x7800}
#define CAT_D_ONZE      {9, 0xF800}
#define CAT_DOTZE       {9, 0x07C0}
#define CAT_NOU         {9, 0x0038}
#define CAT_DEU         {9, 0x0007}

#define CAT_BEN         {12, 0xE000}
#define CAT_TOCADES     {12, 0x0FE0}
#define CAT_EN_PUNT     {12, 0x601E}

#define CAT_TOCADA      {13, 0xFC00}

#define CAT_DE_F        {14, 0xC000}
#define CAT_DEL         {14, 0xE000}
#define CAT_LA_F        {14, 0x1800}
#define CAT_MATI        {14, 0x0780}
#define CAT_NIT         {14, 0x0070}
#define CAT_TARDA       {14, 0x001F}

// SPANISH

#define ESP_ES          {0, 0x0C00}
#define ESP_SON         {0, 0x0700}
#define ESP_CASI        {0, 0x000F}

#define ESP_LA          {4, 0x0600}
#define ESP_LAS         {4, 0x0700}

#define ESP_CINCO       {5, 0xF800}
#define ESP_OCHO        {5, 0x0F00}
#define ESP_UNA         {5, 0x0007}

#define ESP_DOS         {6, 0xE000}
#define ESP_NUEVE       {6, 0x1F00}
#define ESP_DIEZ        {6, 0x00F0}
#define ESP_ONCE        {6, 0x000F}

#define ESP_TRES        {7, 0x1E00}
#define ESP_SEIS        {7, 0x03C0}
#define ESP_CUATRO      {7, 0x003F}

#define ESP_SIETE       {8, 0xF800}
#define ESP_DOCE        {8, 0x0780}
#define ESP_Y           {8, 0x0020}
#define ESP_MENOS       {8, 0x001F}

#define ESP_VEINTICINCO {10, 0xFFE0}
#define ESP_CINCO_B     {10, 0x03E0}
#define ESP_MEDIA       {10, 0x001F}

#define ESP_CUARTO      {11, 0xFC00}
#define ESP_DIEZ_B      {11, 0x03C0}
#define ESP_VEINTE      {11, 0x003F}

#define ESP_EN_PUNTO    {12, 0x601F}

#define ESP_PASADA      {13, 0x03F0}
#define ESP_PASADAS     {13, 0x03F8}

#define ESP_DE_F        {14, 0xC000}
#define ESP_LA_F        {14, 0x1800}

#define ESP_MANANA      {15, 0xFC00}
#define ESP_TARDE       {15, 0x03E0}
#define ESP_NOCHE       {15, 0x001F}


#endif
