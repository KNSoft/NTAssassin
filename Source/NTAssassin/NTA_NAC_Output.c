﻿// This file generated by NTAssassin Auxiliary Compiler V1.0.0.0
// Do not modify manually

#include "include\NTAssassin\NTAssassin.h"

BYTE SYM_Hijack_LoadProcAddr_InjectThread_x86[982] = {
	0x55, 0x8B, 0xEC, 0x8B, 0x45, 0x08, 0x83, 0xEC, 0x38, 0x8B, 0x08, 0x53, 0x56, 0x57, 0x83, 0xF9, 0x04, 0x72, 0x23, 0x8D, 0x58, 0x04, 0xBA, 0x00, 0x00,
	0x00, 0x00, 0x89, 0x5D, 0xC8, 0x83, 0xE9, 0x04, 0x74, 0x13, 0x8D, 0x04, 0x12, 0x42, 0x66, 0x83, 0x3C, 0x18, 0x00, 0x89, 0x55, 0xF8, 0x74, 0x13, 0x83,
	0xE9, 0x01, 0x75, 0xED, 0xB8, 0x0D, 0x00, 0x00, 0xC0, 0x5F, 0x5E, 0x5B, 0x8B, 0xE5, 0x5D, 0xC2, 0x04, 0x00, 0x83, 0xF9, 0x02, 0x72, 0xED, 0x8D, 0x3C,
	0x12, 0x83, 0xE9, 0x02, 0x0F, 0xB7, 0x04, 0x1F, 0x8D, 0x73, 0x02, 0x03, 0xF7, 0x89, 0x7D, 0xD0, 0x89, 0x75, 0xD8, 0x66, 0x85, 0xC0, 0x74, 0x11, 0x89,
	0x45, 0xFC, 0x83, 0xF9, 0x04, 0x72, 0xCB, 0xC7, 0x45, 0xE0, 0x00, 0x00, 0x00, 0x00, 0xEB, 0x64, 0x8D, 0x5E, 0x03, 0x83, 0xE3, 0xFC, 0x8B, 0xC3, 0x89,
	0x5D, 0xFC, 0x2B, 0xC6, 0x3B, 0xC8, 0x72, 0xB1, 0x2B, 0xC8, 0x80, 0x3B, 0x00, 0x75, 0x17, 0xC7, 0x45, 0xE0, 0x00, 0x00, 0x00, 0x00, 0xC7, 0x45, 0xFC,
	0x00, 0x00, 0x00, 0x00, 0xC7, 0x45, 0xD8, 0x00, 0x00, 0x00, 0x00, 0xEB, 0x35, 0x33, 0xF6, 0x85, 0xC9, 0x74, 0x8D, 0x0F, 0x1F, 0x80, 0x00, 0x00, 0x00,
	0x00, 0x8D, 0x04, 0x1E, 0x46, 0x80, 0x38, 0x00, 0x89, 0x75, 0xE0, 0x74, 0x13, 0x83, 0xE9, 0x01, 0x75, 0xEF, 0x5F, 0x5E, 0xB8, 0x0D, 0x00, 0x00, 0xC0,
	0x5B, 0x8B, 0xE5, 0x5D, 0xC2, 0x04, 0x00, 0x8D, 0x04, 0x1E, 0x89, 0x55, 0xF8, 0x89, 0x45, 0xD8, 0x64, 0xA1, 0x30, 0x00, 0x00, 0x00, 0xC7, 0x45, 0xEC,
	0x00, 0x00, 0x00, 0x00, 0x8B, 0x40, 0x0C, 0x8B, 0x40, 0x1C, 0x8B, 0x40, 0x08, 0x89, 0x45, 0x08, 0x64, 0xA1, 0x30, 0x00, 0x00, 0x00, 0x8B, 0x40, 0x0C,
	0x8B, 0x70, 0x0C, 0x8B, 0xCE, 0x89, 0x75, 0xCC, 0x89, 0x75, 0xE4, 0x0F, 0xB7, 0x41, 0x2E, 0x3B, 0xC7, 0x75, 0x78, 0x33, 0xDB, 0x85, 0xD2, 0x74, 0x6A,
	0x8B, 0x41, 0x30, 0x8B, 0x7D, 0xC8, 0x2B, 0xC7, 0x89, 0x45, 0xE8, 0x66, 0x90, 0x0F, 0xB7, 0x17, 0x0F, 0xB7, 0x04, 0x38, 0x8B, 0xCA, 0x66, 0x3B, 0xD0,
	0x74, 0x33, 0x8D, 0x42, 0x9F, 0x8B, 0xF1, 0x66, 0x83, 0xF8, 0x19, 0x77, 0x11, 0x8B, 0x55, 0xE8, 0x8D, 0x4E, 0xE0, 0x0F, 0xB7, 0x04, 0x3A, 0x0F, 0xB7,
	0x17, 0x3B, 0xC8, 0x75, 0x27, 0x8D, 0x42, 0xBF, 0x66, 0x83, 0xF8, 0x19, 0x77, 0x0E, 0x8B, 0x55, 0xE8, 0x8D, 0x4E, 0x20, 0x0F, 0xB7, 0x04, 0x3A, 0x3B,
	0xC8, 0x75, 0x10, 0x8B, 0x55, 0xF8, 0x43, 0x8B, 0x45, 0xE8, 0x83, 0xC7, 0x02, 0x3B, 0xDA, 0x72, 0xB1, 0xEB, 0x03, 0x8B, 0x55, 0xF8, 0x8B, 0x4D, 0xE4,
	0x8B, 0x75, 0xCC, 0x8B, 0x7D, 0xD0, 0x3B, 0xDA, 0x0F, 0x84, 0x99, 0x00, 0x00, 0x00, 0x8B, 0x09, 0x89, 0x4D, 0xE4, 0x3B, 0xCE, 0x0F, 0x85, 0x73, 0xFF,
	0xFF, 0xFF, 0x8B, 0x7D, 0xEC, 0x8B, 0x4D, 0x08, 0x33, 0xDB, 0x8B, 0x55, 0x08, 0x33, 0xF6, 0x89, 0x75, 0xD0, 0x89, 0x5D, 0xCC, 0x8B, 0x41, 0x3C, 0x89,
	0x5D, 0xE4, 0x8B, 0x4C, 0x08, 0x78, 0x89, 0x4D, 0xE8, 0x8B, 0x4C, 0x11, 0x20, 0x03, 0xCA, 0x89, 0x4D, 0xF0, 0x8B, 0x4D, 0xE8, 0x39, 0x5C, 0x11, 0x18,
	0x8B, 0x55, 0xF8, 0x0F, 0x86, 0x61, 0x01, 0x00, 0x00, 0x8B, 0x4D, 0x08, 0x8B, 0x44, 0x08, 0x7C, 0x03, 0x45, 0xE8, 0x8B, 0x4D, 0xF0, 0x89, 0x45, 0xD4,
	0x0F, 0x1F, 0x44, 0x00, 0x00, 0x8B, 0x09, 0x8D, 0x41, 0x17, 0x3B, 0x45, 0xD4, 0x0F, 0x83, 0x3E, 0x01, 0x00, 0x00, 0x85, 0xFF, 0x75, 0x4F, 0x85, 0xF6,
	0x75, 0x4B, 0x8B, 0x45, 0x08, 0x81, 0x3C, 0x01, 0x4C, 0x64, 0x72, 0x4C, 0x75, 0x3F, 0x81, 0x7C, 0x01, 0x04, 0x6F, 0x61, 0x64, 0x44, 0x75, 0x35, 0x81,
	0x7C, 0x01, 0x07, 0x44, 0x6C, 0x6C, 0x00, 0x75, 0x2B, 0x8D, 0x75, 0xD0, 0xE9, 0xA0, 0x00, 0x00, 0x00, 0x8B, 0x79, 0x18, 0x89, 0x7D, 0xEC, 0x85, 0xFF,
	0x0F, 0x84, 0x69, 0xFF, 0xFF, 0xFF, 0x83, 0x7D, 0xFC, 0x00, 0x0F, 0x85, 0x5F, 0xFF, 0xFF, 0xFF, 0x5F, 0x5E, 0x33, 0xC0, 0x5B, 0x8B, 0xE5, 0x5D, 0xC2,
	0x04, 0x00, 0x8B, 0x45, 0xFC, 0x85, 0xC0, 0x0F, 0x84, 0xAF, 0x00, 0x00, 0x00, 0x85, 0xDB, 0x0F, 0x85, 0xA7, 0x00, 0x00, 0x00, 0x8B, 0x45, 0x08, 0x81,
	0x3C, 0x01, 0x4C, 0x64, 0x72, 0x47, 0x8B, 0x45, 0xFC, 0x0F, 0x85, 0x94, 0x00, 0x00, 0x00, 0x8B, 0x45, 0x08, 0x81, 0x7C, 0x01, 0x04, 0x65, 0x74, 0x50,
	0x72, 0x8B, 0x45, 0xFC, 0x0F, 0x85, 0x80, 0x00, 0x00, 0x00, 0x8B, 0x45, 0x08, 0x81, 0x7C, 0x01, 0x08, 0x6F, 0x63, 0x65, 0x64, 0x8B, 0x45, 0xFC, 0x75,
	0x70, 0x8B, 0x45, 0x08, 0x81, 0x7C, 0x01, 0x0B, 0x64, 0x75, 0x72, 0x65, 0x8B, 0x45, 0xFC, 0x75, 0x60, 0x8B, 0x45, 0x08, 0x81, 0x7C, 0x01, 0x0F, 0x41,
	0x64, 0x64, 0x72, 0x8B, 0x45, 0xFC, 0x75, 0x50, 0x8B, 0x45, 0x08, 0x81, 0x7C, 0x01, 0x13, 0x65, 0x73, 0x73, 0x00, 0x8B, 0x45, 0xFC, 0x75, 0x40, 0x8D,
	0x75, 0xCC, 0x8B, 0x45, 0xE4, 0x8B, 0x4D, 0x08, 0x8B, 0x5D, 0xE8, 0x8D, 0x0C, 0x41, 0x8B, 0x45, 0x08, 0x8B, 0x44, 0x03, 0x24, 0x0F, 0xB7, 0x0C, 0x01,
	0x8B, 0x45, 0x08, 0x8B, 0x44, 0x03, 0x1C, 0x8D, 0x04, 0x88, 0x8B, 0x4D, 0x08, 0x8B, 0x04, 0x08, 0x3B, 0xC3, 0x72, 0x05, 0x3B, 0x45, 0xD4, 0x72, 0x3E,
	0x03, 0xC1, 0x89, 0x06, 0x8B, 0x75, 0xD0, 0x8B, 0x5D, 0xCC, 0x8B, 0x45, 0xFC, 0x85, 0xFF, 0x75, 0x04, 0x85, 0xF6, 0x74, 0x08, 0x85, 0xC0, 0x74, 0x33,
	0x85, 0xDB, 0x75, 0x2F, 0x8B, 0x4D, 0xE8, 0x8B, 0x55, 0x08, 0x8B, 0x45, 0xE4, 0x83, 0x45, 0xF0, 0x04, 0x40, 0x89, 0x45, 0xE4, 0x3B, 0x44, 0x11, 0x18,
	0x8B, 0x55, 0xF8, 0x8B, 0x4D, 0xF0, 0x0F, 0x82, 0xB4, 0xFE, 0xFF, 0xFF, 0x5F, 0x5E, 0xB8, 0xE5, 0x00, 0x00, 0xC0, 0x5B, 0x8B, 0xE5, 0x5D, 0xC2, 0x04,
	0x00, 0x33, 0xC0, 0x85, 0xFF, 0x75, 0x48, 0x8D, 0x04, 0x12, 0x66, 0x89, 0x45, 0xF6, 0x83, 0xC0, 0xFE, 0x66, 0x89, 0x45, 0xF4, 0x8B, 0x45, 0xC8, 0x89,
	0x45, 0xF8, 0x8D, 0x45, 0xEC, 0x50, 0x8D, 0x45, 0xF4, 0x50, 0x57, 0x57, 0xFF, 0xD6, 0x3D, 0x42, 0x01, 0x00, 0xC0, 0x75, 0x16, 0x8D, 0x45, 0xEC, 0xC7,
	0x45, 0xC8, 0x02, 0x00, 0x00, 0x00, 0x50, 0x8D, 0x45, 0xF4, 0x50, 0x8D, 0x45, 0xC8, 0x50, 0x57, 0xFF, 0xD6, 0x85, 0xC0, 0x0F, 0x88, 0xB6, 0xFC, 0xFF,
	0xFF, 0x8B, 0x7D, 0xEC, 0x8B, 0x4D, 0xFC, 0x85, 0xC9, 0x0F, 0x84, 0xA8, 0xFC, 0xFF, 0xFF, 0x81, 0xF9, 0xFF, 0xFF, 0x00, 0x00, 0x76, 0x28, 0x8B, 0x45,
	0xE0, 0xFF, 0x75, 0xD8, 0x0F, 0xB7, 0xC0, 0x66, 0x89, 0x45, 0xDE, 0x48, 0x89, 0x4D, 0xE0, 0x33, 0xC9, 0x66, 0x89, 0x45, 0xDC, 0x8D, 0x45, 0xDC, 0x51,
	0x50, 0x57, 0xFF, 0xD3, 0x5F, 0x5E, 0x5B, 0x8B, 0xE5, 0x5D, 0xC2, 0x04, 0x00, 0xFF, 0x75, 0xD8, 0x33, 0xC0, 0x51, 0x50, 0x57, 0xFF, 0xD3, 0x5F, 0x5E,
	0x5B, 0x8B, 0xE5, 0x5D, 0xC2, 0x04, 0x00
};

BYTE SYM_Hijack_LoadProcAddr_InjectThread_x64[1120] = {
	0x40, 0x55, 0x56, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x48, 0x81, 0xEC, 0xA0, 0x00, 0x00, 0x00, 0x8B, 0x11, 0x48, 0x83, 0xFA, 0x04, 0x72, 0x2C, 0x48,
	0x8D, 0x69, 0x04, 0x41, 0xBD, 0x00, 0x00, 0x00, 0x00, 0x45, 0x8B, 0xDD, 0x48, 0x83, 0xEA, 0x04, 0x74, 0x19, 0x0F, 0x1F, 0x44, 0x00, 0x00, 0x41, 0x8B,
	0xC3, 0x41, 0xFF, 0xC3, 0x66, 0x44, 0x39, 0x6C, 0x45, 0x00, 0x74, 0x1B, 0x48, 0x83, 0xEA, 0x01, 0x75, 0xEC, 0xB8, 0x0D, 0x00, 0x00, 0xC0, 0x48, 0x81,
	0xC4, 0xA0, 0x00, 0x00, 0x00, 0x41, 0x5E, 0x41, 0x5D, 0x41, 0x5C, 0x5E, 0x5D, 0xC3, 0x48, 0x83, 0xFA, 0x02, 0x72, 0xE5, 0x4F, 0x8D, 0x24, 0x1B, 0x48,
	0x83, 0xEA, 0x02, 0x41, 0x0F, 0xB7, 0x04, 0x2C, 0x4C, 0x8D, 0x45, 0x02, 0x4D, 0x03, 0xC4, 0x4C, 0x89, 0x44, 0x24, 0x28, 0x66, 0x85, 0xC0, 0x74, 0x0D,
	0x8B, 0xF0, 0x48, 0x83, 0xFA, 0x08, 0x72, 0xBF, 0x45, 0x8B, 0xF5, 0xEB, 0x58, 0x49, 0x8D, 0x70, 0x03, 0x48, 0x83, 0xE6, 0xFC, 0x48, 0x8B, 0xC6, 0x49,
	0x2B, 0xC0, 0x48, 0x3B, 0xD0, 0x72, 0xA7, 0x48, 0x2B, 0xD0, 0x45, 0x8B, 0xF5, 0x44, 0x38, 0x2E, 0x75, 0x0A, 0x49, 0x8B, 0xF5, 0x4C, 0x89, 0x6C, 0x24,
	0x28, 0xEB, 0x30, 0x48, 0x85, 0xD2, 0x74, 0x8D, 0x66, 0x0F, 0x1F, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x8B, 0xC6, 0x41, 0xFF, 0xC6, 0x44, 0x38,
	0x2C, 0x30, 0x74, 0x0B, 0x48, 0x83, 0xEA, 0x01, 0x75, 0xEE, 0xE9, 0x6D, 0xFF, 0xFF, 0xFF, 0x45, 0x8B, 0xC6, 0x4C, 0x03, 0xC6, 0x4C, 0x89, 0x44, 0x24,
	0x28, 0x4C, 0x89, 0xAC, 0x24, 0xE8, 0x00, 0x00, 0x00, 0x65, 0x48, 0x8B, 0x04, 0x25, 0x60, 0x00, 0x00, 0x00, 0x48, 0x89, 0x9C, 0x24, 0x98, 0x00, 0x00,
	0x00, 0x48, 0x89, 0xBC, 0x24, 0x90, 0x00, 0x00, 0x00, 0x4C, 0x89, 0xBC, 0x24, 0x88, 0x00, 0x00, 0x00, 0x48, 0x8B, 0x48, 0x18, 0x48, 0x8B, 0x41, 0x30,
	0x48, 0x8B, 0x78, 0x10, 0x65, 0x48, 0x8B, 0x04, 0x25, 0x60, 0x00, 0x00, 0x00, 0x48, 0x89, 0x7C, 0x24, 0x20, 0x48, 0x8B, 0x48, 0x18, 0x4C, 0x8B, 0x79,
	0x10, 0x49, 0x8B, 0xDF, 0x0F, 0xB7, 0x43, 0x5A, 0x49, 0x3B, 0xC4, 0x75, 0x65, 0x45, 0x8B, 0xCD, 0x45, 0x85, 0xDB, 0x74, 0x54, 0x48, 0x8B, 0x7B, 0x60,
	0x66, 0x66, 0x66, 0x0F, 0x1F, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x8B, 0xC1, 0x48, 0x8D, 0x0C, 0x00, 0x0F, 0xB7, 0x04, 0x29, 0x44, 0x0F, 0xB7,
	0x14, 0x39, 0x66, 0x41, 0x3B, 0xC2, 0x74, 0x27, 0x8D, 0x48, 0x9F, 0x44, 0x8B, 0xC0, 0x66, 0x83, 0xF9, 0x19, 0x77, 0x08, 0x8D, 0x50, 0xE0, 0x41, 0x3B,
	0xD2, 0x75, 0x1B, 0x66, 0x83, 0xE8, 0x41, 0x66, 0x83, 0xF8, 0x19, 0x77, 0x09, 0x41, 0x8D, 0x48, 0x20, 0x41, 0x3B, 0xCA, 0x75, 0x08, 0x41, 0xFF, 0xC1,
	0x45, 0x3B, 0xCB, 0x72, 0xBB, 0x45, 0x3B, 0xCB, 0x0F, 0x84, 0xC6, 0x00, 0x00, 0x00, 0x48, 0x8B, 0x1B, 0x49, 0x3B, 0xDF, 0x75, 0x8A, 0x4C, 0x8B, 0x94,
	0x24, 0xE8, 0x00, 0x00, 0x00, 0x4C, 0x8B, 0x4C, 0x24, 0x20, 0x49, 0x8B, 0xDD, 0x48, 0x89, 0x5C, 0x24, 0x30, 0x49, 0x8B, 0xFD, 0x4C, 0x89, 0x6C, 0x24,
	0x38, 0x45, 0x8B, 0xE5, 0x49, 0x63, 0x41, 0x3C, 0x46, 0x8B, 0x84, 0x08, 0x88, 0x00, 0x00, 0x00, 0x4C, 0x89, 0x44, 0x24, 0x58, 0x43, 0x8B, 0x54, 0x08,
	0x18, 0x4B, 0x8D, 0x0C, 0x08, 0x47, 0x8B, 0x7C, 0x08, 0x20, 0x4D, 0x03, 0xF9, 0x48, 0x89, 0x4C, 0x24, 0x50, 0x89, 0x94, 0x24, 0xD8, 0x00, 0x00, 0x00,
	0x85, 0xD2, 0x0F, 0x84, 0x94, 0x01, 0x00, 0x00, 0x42, 0x8B, 0x84, 0x08, 0x8C, 0x00, 0x00, 0x00, 0x4C, 0x03, 0xC0, 0x89, 0x84, 0x24, 0xD0, 0x00, 0x00,
	0x00, 0x4C, 0x89, 0x44, 0x24, 0x60, 0x0F, 0x1F, 0x40, 0x00, 0x66, 0x0F, 0x1F, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00, 0x41, 0x8B, 0x0F, 0x48, 0x8D, 0x41,
	0x17, 0x49, 0x3B, 0xC0, 0x0F, 0x83, 0x60, 0x01, 0x00, 0x00, 0x4D, 0x85, 0xD2, 0x75, 0x77, 0x48, 0x85, 0xDB, 0x75, 0x72, 0x42, 0x81, 0x3C, 0x09, 0x4C,
	0x64, 0x72, 0x4C, 0x75, 0x68, 0x42, 0x81, 0x7C, 0x09, 0x04, 0x6F, 0x61, 0x64, 0x44, 0x75, 0x5D, 0x42, 0x81, 0x7C, 0x09, 0x07, 0x44, 0x6C, 0x6C, 0x00,
	0x75, 0x52, 0x4C, 0x8D, 0x4C, 0x24, 0x30, 0xE9, 0xAC, 0x00, 0x00, 0x00, 0x4C, 0x8B, 0x53, 0x30, 0x4C, 0x89, 0x94, 0x24, 0xE8, 0x00, 0x00, 0x00, 0x4D,
	0x85, 0xD2, 0x0F, 0x84, 0x35, 0xFF, 0xFF, 0xFF, 0x48, 0x85, 0xF6, 0x0F, 0x85, 0x2C, 0xFF, 0xFF, 0xFF, 0x33, 0xC0, 0x48, 0x8B, 0xBC, 0x24, 0x90, 0x00,
	0x00, 0x00, 0x48, 0x8B, 0x9C, 0x24, 0x98, 0x00, 0x00, 0x00, 0x4C, 0x8B, 0xBC, 0x24, 0x88, 0x00, 0x00, 0x00, 0x48, 0x81, 0xC4, 0xA0, 0x00, 0x00, 0x00,
	0x41, 0x5E, 0x41, 0x5D, 0x41, 0x5C, 0x5E, 0x5D, 0xC3, 0x48, 0x85, 0xF6, 0x0F, 0x84, 0xB7, 0x00, 0x00, 0x00, 0x48, 0x85, 0xFF, 0x0F, 0x85, 0xAE, 0x00,
	0x00, 0x00, 0x42, 0x81, 0x3C, 0x09, 0x4C, 0x64, 0x72, 0x47, 0x0F, 0x85, 0xA0, 0x00, 0x00, 0x00, 0x42, 0x81, 0x7C, 0x09, 0x04, 0x65, 0x74, 0x50, 0x72,
	0x0F, 0x85, 0x91, 0x00, 0x00, 0x00, 0x42, 0x81, 0x7C, 0x09, 0x08, 0x6F, 0x63, 0x65, 0x64, 0x0F, 0x85, 0x82, 0x00, 0x00, 0x00, 0x42, 0x81, 0x7C, 0x09,
	0x0B, 0x64, 0x75, 0x72, 0x65, 0x75, 0x77, 0x42, 0x81, 0x7C, 0x09, 0x0F, 0x41, 0x64, 0x64, 0x72, 0x75, 0x6C, 0x42, 0x81, 0x7C, 0x09, 0x13, 0x65, 0x73,
	0x73, 0x00, 0x75, 0x61, 0x4C, 0x8D, 0x4C, 0x24, 0x38, 0x48, 0x8B, 0x5C, 0x24, 0x50, 0x48, 0x8B, 0x7C, 0x24, 0x20, 0x41, 0x8B, 0xD4, 0x8B, 0x4B, 0x24,
	0x48, 0x03, 0xCF, 0x44, 0x0F, 0xB7, 0x04, 0x51, 0x8B, 0x4B, 0x1C, 0x48, 0x03, 0xCF, 0x42, 0x8B, 0x14, 0x81, 0x4C, 0x8B, 0x44, 0x24, 0x58, 0x41, 0x3B,
	0xD0, 0x72, 0x0E, 0x8B, 0x84, 0x24, 0xD0, 0x00, 0x00, 0x00, 0x41, 0x03, 0xC0, 0x3B, 0xD0, 0x72, 0x46, 0x4C, 0x8B, 0x44, 0x24, 0x60, 0x48, 0x8D, 0x04,
	0x17, 0x8B, 0x94, 0x24, 0xD8, 0x00, 0x00, 0x00, 0x49, 0x89, 0x01, 0x48, 0x8B, 0x5C, 0x24, 0x30, 0x48, 0x8B, 0x7C, 0x24, 0x38, 0x4C, 0x8B, 0x4C, 0x24,
	0x20, 0x4D, 0x85, 0xD2, 0x75, 0x05, 0x48, 0x85, 0xDB, 0x74, 0x0A, 0x48, 0x85, 0xF6, 0x74, 0x1F, 0x48, 0x85, 0xFF, 0x75, 0x1A, 0x41, 0xFF, 0xC4, 0x49,
	0x83, 0xC7, 0x04, 0x44, 0x3B, 0xE2, 0x0F, 0x82, 0x90, 0xFE, 0xFF, 0xFF, 0xB8, 0xE5, 0x00, 0x00, 0xC0, 0xE9, 0xEA, 0xFE, 0xFF, 0xFF, 0x41, 0x8B, 0xC5,
	0x4D, 0x85, 0xD2, 0x75, 0x68, 0x66, 0x45, 0x03, 0xDB, 0x48, 0x89, 0x6C, 0x24, 0x48, 0x66, 0x44, 0x89, 0x5C, 0x24, 0x42, 0x4C, 0x8D, 0x8C, 0x24, 0xE8,
	0x00, 0x00, 0x00, 0x66, 0x41, 0x83, 0xEB, 0x02, 0x4C, 0x8D, 0x44, 0x24, 0x40, 0x33, 0xD2, 0x66, 0x44, 0x89, 0x5C, 0x24, 0x40, 0x33, 0xC9, 0xFF, 0xD3,
	0x3D, 0x42, 0x01, 0x00, 0xC0, 0x75, 0x24, 0x4C, 0x8D, 0x8C, 0x24, 0xE8, 0x00, 0x00, 0x00, 0xC7, 0x84, 0x24, 0xE0, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
	0x00, 0x4C, 0x8D, 0x44, 0x24, 0x40, 0x33, 0xC9, 0x48, 0x8D, 0x94, 0x24, 0xE0, 0x00, 0x00, 0x00, 0xFF, 0xD3, 0x85, 0xC0, 0x0F, 0x88, 0x82, 0xFE, 0xFF,
	0xFF, 0x4C, 0x8B, 0x94, 0x24, 0xE8, 0x00, 0x00, 0x00, 0x48, 0x85, 0xF6, 0x0F, 0x84, 0x71, 0xFE, 0xFF, 0xFF, 0x48, 0x81, 0xFE, 0xFF, 0xFF, 0x00, 0x00,
	0x76, 0x2C, 0x4C, 0x8B, 0x4C, 0x24, 0x28, 0x48, 0x8D, 0x54, 0x24, 0x68, 0x66, 0x44, 0x89, 0x74, 0x24, 0x6A, 0x45, 0x8B, 0xC5, 0x66, 0x41, 0xFF, 0xCE,
	0x48, 0x89, 0x74, 0x24, 0x70, 0x49, 0x8B, 0xCA, 0x66, 0x44, 0x89, 0x74, 0x24, 0x68, 0xFF, 0xD7, 0xE9, 0x3C, 0xFE, 0xFF, 0xFF, 0x4C, 0x8B, 0x4C, 0x24,
	0x28, 0x49, 0x8B, 0xD5, 0x44, 0x8B, 0xC6, 0x49, 0x8B, 0xCA, 0x44, 0x8B, 0xEE, 0xFF, 0xD7, 0xE9, 0x24, 0xFE, 0xFF, 0xFF
};

BYTE SYM_Hijack_CallProc_InjectThread_x86[138] = {
	0x55, 0x8B, 0xEC, 0x53, 0x57, 0x56, 0x33, 0xC0, 0x8B, 0x7D, 0x08, 0x83, 0x7F, 0x08, 0x00, 0x74, 0x0C, 0xB8, 0x02, 0x00, 0x00, 0xC0, 0x5E, 0x5F, 0x5B,
	0xC9, 0xC2, 0x04, 0x00, 0x8D, 0x77, 0x24, 0x8B, 0x4F, 0x20, 0xB8, 0x14, 0x00, 0x00, 0x00, 0xF7, 0xE1, 0x8D, 0x1C, 0x06, 0x8B, 0x46, 0x08, 0x0B, 0xC0,
	0x74, 0x11, 0x83, 0xF8, 0xFF, 0x74, 0x0C, 0x8B, 0xD3, 0x83, 0xC0, 0x03, 0x83, 0xE0, 0xFC, 0x03, 0xD8, 0xEB, 0x02, 0x8B, 0x16, 0x52, 0x83, 0xC6, 0x14,
	0xE2, 0xE0, 0x33, 0xC0, 0x64, 0xA3, 0x34, 0x00, 0x00, 0x00, 0x64, 0xA3, 0xF4, 0x0B, 0x00, 0x00, 0x64, 0xA3, 0xA4, 0x01, 0x00, 0x00, 0xFF, 0x17, 0x89,
	0x47, 0x0C, 0x64, 0xA1, 0x34, 0x00, 0x00, 0x00, 0x89, 0x47, 0x14, 0x64, 0xA1, 0xF4, 0x0B, 0x00, 0x00, 0x89, 0x47, 0x18, 0x64, 0xA1, 0xA4, 0x01, 0x00,
	0x00, 0x89, 0x47, 0x1C, 0x33, 0xC0, 0x5E, 0x5F, 0x5B, 0xC9, 0xC2, 0x04, 0x00
};

BYTE SYM_Hijack_CallProc_InjectThread_x64[266] = {
	0x53, 0x57, 0x56, 0x48, 0x8B, 0xF9, 0x83, 0x7F, 0x08, 0x00, 0x74, 0x09, 0xB8, 0x02, 0x00, 0x00, 0xC0, 0x5E, 0x5F, 0x5B, 0xC3, 0x48, 0x8D, 0x77, 0x24,
	0x44, 0x8B, 0x57, 0x20, 0x48, 0xC7, 0xC0, 0x14, 0x00, 0x00, 0x00, 0x41, 0xF7, 0xE2, 0x48, 0x8D, 0x1C, 0x06, 0x45, 0x0B, 0xD2, 0x0F, 0x84, 0x8F, 0x00,
	0x00, 0x00, 0x48, 0x8B, 0x46, 0x08, 0x48, 0x0B, 0xC0, 0x74, 0x19, 0x48, 0x83, 0xF8, 0xFF, 0x74, 0x13, 0x4C, 0x8B, 0xDB, 0x48, 0x83, 0xC0, 0x03, 0x48,
	0x83, 0xE0, 0xFC, 0x48, 0x03, 0xD8, 0x48, 0x33, 0xC0, 0xEB, 0x03, 0x4C, 0x8B, 0x1E, 0x41, 0x83, 0xFA, 0x04, 0x75, 0x11, 0x48, 0x0B, 0xC0, 0x74, 0x07,
	0x66, 0x49, 0x0F, 0x6E, 0xDB, 0xEB, 0x4C, 0x4D, 0x8B, 0xCB, 0xEB, 0x47, 0x41, 0x83, 0xFA, 0x03, 0x75, 0x11, 0x48, 0x0B, 0xC0, 0x74, 0x07, 0x66, 0x49,
	0x0F, 0x6E, 0xD3, 0xEB, 0x35, 0x4D, 0x8B, 0xC3, 0xEB, 0x30, 0x41, 0x83, 0xFA, 0x02, 0x75, 0x11, 0x48, 0x0B, 0xC0, 0x74, 0x07, 0x66, 0x49, 0x0F, 0x6E,
	0xCB, 0xEB, 0x1E, 0x49, 0x8B, 0xD3, 0xEB, 0x19, 0x41, 0x83, 0xFA, 0x01, 0x75, 0x11, 0x48, 0x0B, 0xC0, 0x74, 0x07, 0x66, 0x49, 0x0F, 0x6E, 0xC3, 0xEB,
	0x07, 0x49, 0x8B, 0xCB, 0xEB, 0x02, 0x41, 0x53, 0x48, 0x83, 0xC6, 0x14, 0x41, 0xFF, 0xCA, 0xE9, 0x68, 0xFF, 0xFF, 0xFF, 0x33, 0xC0, 0x65, 0x89, 0x04,
	0x25, 0x68, 0x00, 0x00, 0x00, 0x65, 0x89, 0x04, 0x25, 0x48, 0x12, 0x00, 0x00, 0x65, 0x89, 0x04, 0x25, 0xC0, 0x02, 0x00, 0x00, 0xFF, 0x17, 0x48, 0x89,
	0x47, 0x0C, 0x65, 0x8B, 0x04, 0x25, 0x68, 0x00, 0x00, 0x00, 0x89, 0x47, 0x14, 0x65, 0x8B, 0x04, 0x25, 0x48, 0x12, 0x00, 0x00, 0x89, 0x47, 0x18, 0x65,
	0x8B, 0x04, 0x25, 0xC0, 0x02, 0x00, 0x00, 0x89, 0x47, 0x1C, 0x33, 0xC0, 0x5E, 0x5F, 0x5B, 0xC3
};

