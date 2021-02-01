// ONLY SHARD DEPENDENT DATA:
const SHARD			:= 0;	// Which shard is THIS ONE?
const MACRO_HOUR_START		:= 0;	// Starting macro hour 24h00 day style. Use 0 for midnight
const MACRO_HOUR_END		:= 0;	// Ending macro hour 24h00 day style. If both are equals, then macro time will be allowed
const ON_SUMMER_HOUR		:= 0;	// If 1, increase the effective hour by one. America only

function ShardName()
	return( GetShardsData()[SHARD][1] );
endfunction

function ShardTimeZone()
	return( 0 );
endfunction
