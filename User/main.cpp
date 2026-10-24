#include "imports.h"

auto cachethread() -> void
{
	auto pml4base = driver.pml4_base();
	printf( "[Deox] PML4 Base: 0x%p\n", pml4base);

	while (true)
	{
		auto uworld = driver.read< uintptr_t >( pml4base + offsets::uworld );
		printf( "[Deox] UWorld: 0x%p\n", uworld );

		auto ulevel = driver.read< uintptr_t >( uworld  + offsets::ulevel );
		printf( "[Deox] ULevel: 0x%p\n", ulevel );

		auto gamestate = driver.read< uintptr_t >( uworld + offsets::gamestate );
		printf( "[Deox] GameState: 0x%p\n", gamestate );

		Sleep( 2000 );
	}
}

auto main() -> const NTSTATUS
{
	printf( R"(
    ____                 
   / __ \___  ____  _  __
  / / / / _ \/ __ \| |/_/
 / /_/ /  __/ /_/ />  <  
/_____/\___/\____/_/|_|  
                         
Shadow Memory Region Driver
)" );

	auto process = utils::getprocessid( L"VALORANT-Win64-Shipping.exe" );

	printf( "[Deox] Target Process ID: %i\n", process );

	if ( process != 0 )
	{
		driver.initdriver( process );
		std::thread(cachethread).detach();
	}

	getchar();
	return 0;
}
