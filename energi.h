const uint32_t energi_width = 40;
const uint32_t energi_height = 40;
const uint8_t energi_data[(40*40)/2] = {
0xFF, 0xFA, 0xEF, 0xFA, 0xFF, 0xFF, 0xFF, 0x9D, 0x77, 0x97, 0xFD, 0xFF, 0xFF, 0xFF, 0x99, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xD7, 0xCF, 0xF8, 0xFF, 0xFF, 0x9F, 0xB7, 0xED, 0xAD, 0x96, 0xFF, 0xFF, 0xAF, 0x88, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xF7, 0xDF, 0xF9, 0xFF, 0xFF, 0x97, 0xFF, 0xFF, 0xFF, 0x8F, 0xF8, 0xFF, 0x7A, 0xFF, 0xA8, 0xFF, 0xFF, 0xFF, 0xFF, 
	0x8B, 0x85, 0x79, 0x85, 0xFD, 0x9F, 0xF9, 0xFF, 0xFF, 0x9F, 0xFF, 0xA8, 0xAF, 0xF4, 0xFF, 0x5F, 0xF9, 0xFF, 0xFF, 0xFF, 
	0x97, 0xCD, 0xCC, 0x9D, 0xF9, 0x7D, 0xFF, 0xFF, 0xFF, 0x58, 0xFF, 0x6F, 0x9E, 0x58, 0xED, 0x85, 0xF8, 0xFF, 0xFF, 0xFF, 
	0xD8, 0xFF, 0xFF, 0xBF, 0xF9, 0xA9, 0xFF, 0xFF, 0xCF, 0xA3, 0xFF, 0x9F, 0xFB, 0x9F, 0xDC, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xD8, 0xFF, 0xFF, 0xBF, 0xFA, 0xD8, 0xFF, 0xFF, 0x7E, 0xBA, 0xFF, 0xCF, 0xF9, 0x8F, 0xCC, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xC8, 0xFF, 0xFF, 0xBF, 0xFA, 0xD8, 0xFF, 0xFF, 0xD6, 0xB7, 0xFF, 0xDF, 0xF8, 0xCF, 0xFE, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 
	0x7B, 0xFF, 0xFF, 0x6F, 0xFD, 0xD7, 0xFF, 0x9F, 0xFA, 0x3B, 0xF9, 0xCF, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0x8F, 0xC7, 0xBD, 0xA6, 0xFF, 0xAA, 0xFF, 0x4E, 0xE7, 0x7E, 0xFF, 0x9F, 0xFB, 0xCF, 0xED, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0x8C, 0x95, 0xFD, 0xFF, 0x7D, 0xFF, 0xFF, 0xD7, 0xD7, 0xFF, 0x6F, 0xFF, 0x8F, 0xDC, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xF8, 0xFF, 0xFF, 0x9F, 0xFA, 0xFF, 0x89, 0xF9, 0xFF, 0xA8, 0xFF, 0x9F, 0xCC, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xEF, 0xF7, 0xFF, 0xFF, 0xFF, 0xC6, 0xDF, 0x83, 0xFF, 0xBF, 0xF7, 0x7F, 0x68, 0xED, 0x75, 0xF7, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xEF, 0xF7, 0xFF, 0xFF, 0xFF, 0x8D, 0xBF, 0xF4, 0xFF, 0x6F, 0xFF, 0xBF, 0xE5, 0xFF, 0x6E, 0xFA, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xEF, 0xF8, 0xFF, 0xFF, 0xFF, 0x7F, 0xDF, 0xFC, 0xFF, 0x8D, 0xFF, 0xFF, 0x7B, 0xFF, 0xA7, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xFF, 0xF7, 0xFF, 0xFF, 0xFF, 0x8F, 0xFE, 0xFF, 0xFF, 0x8D, 0xFF, 0xFF, 0xBF, 0x77, 0xFA, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xEF, 0xF7, 0xFF, 0xFF, 0xFF, 0x7F, 0xEC, 0xDD, 0xED, 0x8B, 0xFF, 0xFF, 0xFF, 0xAB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xEF, 0xF7, 0xFF, 0xFF, 0xFF, 0x7F, 0x87, 0x88, 0x87, 0x86, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xEF, 0xF7, 0xFF, 0xFF, 0xFF, 0x8F, 0xFE, 0xFF, 0xFF, 0x9E, 0xFF, 0xEF, 0x9A, 0xEE, 0xA9, 0xFF, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xEF, 0xF7, 0xFF, 0xFF, 0xFF, 0x8F, 0xFD, 0xFF, 0xFF, 0x8C, 0xFF, 0x7F, 0xB9, 0x56, 0x9B, 0xF7, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xEF, 0xF7, 0xFF, 0xFF, 0xFF, 0xAF, 0xF9, 0xFF, 0xFF, 0xB9, 0xFF, 0x8E, 0xFF, 0xAA, 0xFF, 0xD8, 0xFF, 0xFF, 0xFF, 
	0xFF, 0xEF, 0xF7, 0xFF, 0xFF, 0xFF, 0xFF, 0xB6, 0xFF, 0xAF, 0xF7, 0xEF, 0x59, 0xFE, 0x67, 0xEF, 0xA5, 0xFE, 0xFF, 0xFF, 
	0xFF, 0xEF, 0xF7, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x97, 0x77, 0xFF, 0x78, 0xBB, 0x77, 0xBB, 0x78, 0xBB, 0x97, 0xFF, 0xFF, 
	0xFF, 0xEF, 0xF7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x6C, 0xFE, 0x7F, 0xF9, 0xBD, 0x9A, 0xBB, 0xA9, 0xEB, 0x9F, 0xF7, 0xFF, 
	0xFF, 0xFF, 0xF7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x8E, 0xFF, 0x8A, 0x8F, 0xA7, 0xAA, 0xAA, 0xAA, 0x79, 0xF8, 0xA8, 0xFF, 
	0xFF, 0xFF, 0xD7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x9A, 0xFF, 0xF6, 0x99, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x89, 0x6F, 0xFF, 
	0xFF, 0xFF, 0x7C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xD8, 0xBF, 0xF9, 0xF7, 0xFF, 0xFF, 0xBA, 0xFF, 0xFF, 0x7F, 0x9E, 0xFB, 
	0xFF, 0xFF, 0x8F, 0xFC, 0xAD, 0x78, 0x77, 0x88, 0xFA, 0x9F, 0xAE, 0xFA, 0xFF, 0xFF, 0x8A, 0xFF, 0xFF, 0xAF, 0xEB, 0xF9, 
	0xFF, 0xFF, 0xFF, 0x7D, 0xB7, 0xEE, 0xFF, 0xDE, 0x69, 0xFA, 0x8F, 0xFD, 0xFF, 0xAF, 0x86, 0xE8, 0xFF, 0xDF, 0xF8, 0xFF, 
	0xFF, 0xFF, 0x9F, 0xD6, 0xCF, 0x89, 0x78, 0xB9, 0xFF, 0x78, 0x6D, 0xFF, 0xFF, 0x7E, 0xFF, 0xFF, 0xFF, 0xFF, 0xF7, 0xFF, 
	0xFF, 0xFF, 0x87, 0xCF, 0x86, 0xDB, 0xEE, 0x9C, 0x96, 0xDF, 0xA4, 0xFF, 0xFF, 0x7D, 0xFF, 0xFF, 0xFF, 0xFF, 0x8A, 0xFF, 
	0xFF, 0x8F, 0xF9, 0x79, 0xFE, 0xFF, 0xFF, 0xFF, 0xAF, 0xE5, 0x6E, 0xFF, 0xFF, 0x9F, 0xA7, 0xFF, 0xFF, 0xFF, 0x7F, 0xFC, 
	0xFF, 0x7B, 0x7F, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x5E, 0xDE, 0xF6, 0xFF, 0xFF, 0xAD, 0xF7, 0xFF, 0xFF, 0xEF, 0xE6, 
	0xFF, 0xF6, 0x8A, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xDF, 0xF6, 0xA8, 0xFF, 0xFF, 0xFF, 0xC8, 0xFF, 0xFF, 0xFF, 0x8B, 
	0xAF, 0xEA, 0xF6, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x9A, 0x6F, 0xFF, 0xFF, 0xEF, 0xE7, 0xFF, 0xFF, 0xFF, 0x7E, 
	0x7F, 0x8F, 0xFB, 0xFF, 0xFF, 0xDF, 0xAA, 0xFF, 0xFF, 0xCE, 0x6F, 0x8F, 0xFC, 0x8E, 0x66, 0xFC, 0xFF, 0xFF, 0xFF, 0x6F, 
	0x8D, 0x7F, 0xFF, 0xFF, 0xFF, 0x78, 0x9B, 0xD6, 0xAF, 0xA6, 0x9F, 0xDA, 0xF8, 0xFF, 0xAA, 0xFF, 0xFF, 0xFF, 0xFF, 0x7D, 
	0xAB, 0xBF, 0xFF, 0xFF, 0xAF, 0xF8, 0xFF, 0x5E, 0x97, 0xFF, 0xEF, 0xFB, 0xF8, 0xFF, 0xDE, 0xFF, 0xFF, 0xFF, 0xFF, 0xB8, 
	0x8B, 0xEF, 0xEF, 0xFF, 0x7F, 0xFE, 0xEF, 0x7F, 0xFA, 0xFF, 0xFE, 0xBF, 0xF7, 0xFF, 0xFF, 0xEF, 0xFF, 0xFE, 0x8F, 0xF8, 
	0x9F, 0x76, 0x76, 0x66, 0x56, 0x67, 0x76, 0x67, 0x66, 0x67, 0x77, 0x56, 0x76, 0x67, 0x76, 0x76, 0x77, 0x77, 0x96, 0xFF, 
	};
