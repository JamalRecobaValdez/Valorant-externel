#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <string>
#include <memory>
#include <encryption/sk.hxx>
#include <encryption/spoof.h>

constexpr ULONG init_code = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x775, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
constexpr ULONG read_code = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x776, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
constexpr ULONG read_kernel_code = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x777, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
constexpr ULONG mouse_move_code = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x778, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);

namespace km {
	inline HANDLE driver_handle;
	
	struct info_t
	{
		HANDLE target_pid = 0;
		void* cr3_context = 0x0;
		void* target_address = 0x0;
		void* buffer_address = 0x0;
		SIZE_T size = 0;
		SIZE_T return_size = 0;
	};

#pragma pack(push, 1)
	typedef struct _KMOUSE_REQUEST {
		long x;
		long y;
		unsigned char button_flags;
	} KMOUSE_REQUEST, * PKMOUSE_REQUEST;
#pragma pack(pop)
	inline uintptr_t cr3_context = 0;
	inline uintptr_t vgk = 0;
	inline uintptr_t base = 0;
	inline uintptr_t process_id = 0;
	inline bool hvci_enabled = false;

	inline bool init() {
		SPOOF_FUNC;
		driver_handle = CreateFileW(skCrypt(L"\\\\.\\WoMic"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL); 

		if (!driver_handle || (driver_handle == INVALID_HANDLE_VALUE))
			return false;

		return true;
	}

	inline void	mouse_mouse(int x, int y)
	{
		SPOOF_FUNC;
		KMOUSE_REQUEST mouse_req = {};
		mouse_req.x = x;
		mouse_req.y = y;
		mouse_req.button_flags = 0;
		DeviceIoControl(driver_handle, mouse_move_code,
			&mouse_req, sizeof(mouse_req),
			nullptr, 0, // do not allow output buffer for now
			nullptr, nullptr);
	}

	inline	void attach_to_process(DWORD process_id) {
		info_t io_info;

		io_info.target_pid = (HANDLE)process_id;

		DeviceIoControl(driver_handle, init_code, &io_info, sizeof(io_info), &io_info, sizeof(io_info), nullptr, nullptr);
	}

	inline void RPMRAW(uintptr_t address, uintptr_t buffer, size_t size) {
		SPOOF_FUNC;
		info_t io_info;
		io_info.cr3_context = (void*)cr3_context;
		io_info.target_address = (void*)address;
		io_info.buffer_address = (void*)buffer;
		io_info.size = size;

		DeviceIoControl(driver_handle, read_code, &io_info, sizeof(io_info), &io_info, sizeof(io_info), nullptr, nullptr);
	}

	template<typename T>
	inline 	void RPMARRAY(uintptr_t address, T* array, size_t len)
	{
		SPOOF_FUNC;
		RPMRAW(address, (uintptr_t)array, sizeof(T) * len);
	}

	template<typename T> T  inline RPM(uintptr_t address) {
		SPOOF_FUNC;
		info_t io_info;
		T read_data{};

		io_info.cr3_context = (void*)cr3_context;
		io_info.target_address = (void*)address;
		io_info.buffer_address = (void*)&read_data;
		io_info.size = sizeof(T);

		DeviceIoControl(driver_handle, read_code, &io_info, sizeof(io_info), &io_info, sizeof(io_info), nullptr, nullptr);

		return read_data;
	}

	template<typename T> T inline RKA(uintptr_t address, size_t additional_size = 0) {
		SPOOF_FUNC;
		info_t io_info;
		T read_data{};

		io_info.target_address = (void*)address;
		io_info.buffer_address = (void*)&read_data;
		io_info.size = sizeof(T) + additional_size;

		DeviceIoControl(driver_handle, read_kernel_code, &io_info, sizeof(io_info), &io_info, sizeof(io_info), nullptr, nullptr);

		return read_data;
	}

	inline INT32 find_process(LPCTSTR process_name) {
		SPOOF_FUNC;
		PROCESSENTRY32 pt;
		HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		pt.dwSize = sizeof(PROCESSENTRY32);
		if (Process32First(hsnap, &pt)) {
			do {
				if (!lstrcmpi(pt.szExeFile, process_name)) {
					CloseHandle(hsnap);
					process_id = pt.th32ProcessID;
					return pt.th32ProcessID;
				}
			} while (Process32Next(hsnap, &pt));
		}
		CloseHandle(hsnap);
		return { NULL };
	}
}

