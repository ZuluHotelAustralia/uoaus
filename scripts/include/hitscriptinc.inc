// HitScript Include

use uo;

include "include/client";
include "include/dotempmods";
include "include/classes";
include "include/damages";
include "include/bitwise";
include "include/random";


function RecalcDmg( attacker, defender, weapon, armor, byref basedamage, piercing := 0 )
/*
	if( attacker = defender )

		var times := CInt( GetObjProperty( attacker , "#AttackedSelf" ) );

		if( times > 3 )
			SendSysMessage( attacker , "The god won't tolerate such a foolishness..." );
			PlayLightningBoltEffect( attacker );
			SetObjProperty( attacker , "#AttackedSelf" , times + 1 );
			SetRawSkill( attacker , weapon.skillid , 0 );
			SetRawSkill( attacker , SKILLID_TACTICS , 0 );
			SetRawSkill( attacker , SKILLID_WRESTLING , 0 );
			ApplyRawDamage( attacker , attacker.hp+3 );
			return 0;
		elseif( times )
			SetObjProperty( attacker , "#AttackedSelf" , times + 1 );
		else
			SetObjProperty( attacker , "#AttackedSelf" , 1 );
		endif

	endif
*/
	if( !GetObjProperty( weapon, "Astral" ) )
		return RecalcPhysicalDmg( attacker , defender , weapon , armor , basedamage , piercing );
	else
		return RecalcAstralDmg( attacker , defender , weapon , armor , basedamage , piercing );
	endif

endfunction


function RecalcPhysicalDmg( attacker, defender, weapon, armor, byref basedamage, piercing := 0 )

	var rawdamage;
	var absorbed;
	var ar := CInt(armor.ar) + CInt(defender.ar_mod);
	var anatomy := GetEffectiveSkill( attacker , SKILLID_ANATOMY );

	if( defender.npctemplate )
		if( !attacker.npctemplate )
			basedamage := basedamage * 1.5;
		endif
	endif

	basedamage := ModByDist( attacker , defender , weapon , basedamage );
	basedamage := basedamage * ( 1 + anatomy * 0.002 );

	if( !piercing )
		rawdamage := GetShieldAbsorption( attacker , defender , weapon , basedamage );
		absorbed  := ar * (Random(51) + 50) * 0.01;
	else
		rawdamage := basedamage;
		absorbed  := 0;
	endif

	rawdamage := rawdamage - absorbed;
	rawdamage := rawdamage * 0.5;
	rawdamage := ModByProt( defender , rawdamage );

	if( rawdamage < 0 )
		rawdamage := 0;
	endif

	MaceHit( attacker , defender , weapon , armor , absorbed );
//	CheckForTheSkillGain( attacker , defender , weapon );

	return CInt( rawdamage );

endfunction


function RecalcAstralDmg( attacker, defender, weapon, armor, byref basedamage, piercing := 0 )

	var rawdamage;
	var absorbed := 0;

	var ar := CInt( GetObjProperty( armor , "Astral" ) ) * 25 * CInt( armor.ar );

	basedamage := basedamage
		* ((GetEffectiveSkill( attacker , SKILLID_SPIRITSPEAK ) + 50.0 + (attacker.intelligence * 0.2)) * 0.01)
		/ ((GetEffectiveSkill( attacker , SKILLID_TACTICS ) + 50.0 + (attacker.strength * 0.2)) * 0.01);

	AwardPoints( attacker, SKILLID_EVALINT, 30 );
	AwardPoints( attacker, SKILLID_SPIRITSPEAK, 30 );

	if( defender.npctemplate )
		basedamage := basedamage * ClasseBonus( attacker , CLASSEID_MAGE );
	else
		var level := GetObjProperty( attacker , CLASSEID_MAGE );
		if( level )
			level := level - 2;
			if( level >= 1 )
				basedamage := basedamage * ClasseBonusByLevel( level );
			endif
		endif
	endif

	if( GetObjProperty( attacker , CLASSEID_WARRIOR ) )
		if( !GetObjProperty( attacker , CLASSEID_MAGE ) )
			basedamage := basedamage / ClasseBonus( attacker , CLASSEID_WARRIOR );
		endif
	endif

	if( GetObjProperty( defender , CLASSEID_WARRIOR ) )
		if( !GetObjProperty( defender , CLASSEID_MAGE ) )
			basedamage := basedamage * ClasseBonus( defender , CLASSEID_WARRIOR );
		endif
	endif
	
	if( GetObjProperty( defender, CLASSEID_THIEF) )
		if( !GetObjProperty( defender, CLASSEID_MAGE ) )
			basedamage := basedamage * 1.3;
		endif
	endif

	basedamage := basedamage * ( 1 + GetEffectiveSkill( attacker , SKILLID_EVALINT ) * 0.002 );
	rawdamage  := basedamage;

	if( !piercing )

		var meditation := GetEffectiveSkill( defender , SKILLID_MEDITATION );
		meditation := meditation * ClasseBonus( defender , CLASSEID_MAGE );

		if( (Random(100) + 1) <= CInt( meditation * 0.5 ) )

			var resist := GetEffectiveSkill( defender , SKILLID_MAGICRESISTANCE );
			var defint := defender.intelligence;

			AwardPoints( defender, SKILLID_MEDITATION, 30 );
			AwardPoints( defender, SKILLID_MAGICRESISTANCE, 30 );
			PerformAction(  defender, ANIM_CAST_AREA );
			SendSysmessage( defender, "You successfully block a part of the astral attack." );

			if( GetObjProperty( defender, CLASSEID_MAGE ) )
				rawdamage := rawdamage - (resist + defint) * 0.25 * ClasseBonus( defender , CLASSEID_MAGE );
			elseif( GetObjProperty( defender, CLASSEID_WARRIOR ) )
				rawdamage := rawdamage - (resist + defint) * 0.25 / ClasseBonus( defender , CLASSEID_WARRIOR );
			endif

			if( rawdamage < 0 )
				rawdamage := 0;
			endif

		endif

		if( ar )
			absorbed := CInt( ar * (Random(51) + 50) / 100 );
		endif

	endif

	rawdamage := rawdamage - absorbed;
	rawdamage := rawdamage * 0.5;
	if( rawdamage < 0 )
		rawdamage := 0;
	endif

//	CheckForTheSkillGain( attacker , defender , weapon );

	return CInt(rawdamage);

endfunction


function ModByDist( attacker, defender, weapon, basedamage )
var classmod := 0;
	if( weapon.skillid == SKILLID_ARCHERY )
case(getobjproperty(attacker, CLASSEID_RANGER))
1: classmod := 150 * 0.01;
2: classmod := 160 * 0.01;
3: classmod := 170 * 0.01;
4: classmod := 180 * 0.01;
5: classmod := 190 * 0.01;
endcase
             basedamage := basedamage * classmod;
             basedamage := basedamage
			* ((attacker.dexterity + 60.0) * 0.01)
			/ ((GetEffectiveSkill( attacker , SKILLID_ARCHERY ) + 70) * 0.01);

		var dist := Distance( attacker , defender );
		if( (dist <= 1) or (dist > 10) )
			basedamage := basedamage * 0.25;
		endif

		basedamage := basedamage * ClasseBonus( attacker , CLASSEID_RANGER );

	else

		if( defender.npctemplate )
			if( GetObjProperty( attacker , CLASSEID_WARRIOR) )
			basedamage := basedamage * ClasseBonus( attacker , CLASSEID_WARRIOR );
			elseif( GetObjProperty( attacker, CLASSEID_THIEF) )
			basedamage := (basedamage*1.2) * ClasseBonus( attacker , CLASSEID_THIEF);
                        endif
		else
			var level := GetObjProperty( attacker , CLASSEID_WARRIOR );
			if( level )
				level := level - 2;
				if( level >= 1 )
					basedamage := basedamage * ClasseBonusByLevel( level );
				endif
			endif
                        var alevel := GetObjProperty( attacker, CLASSEID_THIEF );
	                       if ( alevel )
		                      alevel := alevel - 1;
		                      if( alevel >= 1 )
                        basedamage := ((basedamage * ClasseBonusByLevel( alevel ) )*1.2) ;
		                      endif
	                       endif
		endif

	endif


	if( GetObjProperty( attacker , CLASSEID_MAGE ) )
		if( !GetObjProperty( attacker , CLASSEID_WARRIOR ) )
			basedamage := basedamage / ClasseBonus( attacker , CLASSEID_MAGE );
		endif
	endif

	if( GetObjProperty( defender , CLASSEID_MAGE ) )
		if( !GetObjProperty( defender , CLASSEID_WARRIOR ) )
			basedamage := basedamage * ClasseBonus( defender , CLASSEID_MAGE );
		endif
	endif

	return basedamage;

endfunction


function MaceHit( attacker, defender, weapon, armor, absorbed )

	var archance := 2;
	var weapchance := 2;

	if( weapon.skillid == SKILLID_MACEFIGHTING )

		var loss := Random(2) + 1;
		if(loss > defender.stamina )
			defender.stamina := 0;
		else
			defender.stamina := defender.stamina - loss;
		endif

		if( armor.hp )
			if( Random(100) < 5 )
				armor.hp := armor.hp - 1;
			endif
		endif

	elseif( Random(100) < archance )

		if( armor.hp )
			armor.hp := armor.hp - 1;
		endif

	endif

	if( Random(100) < weapchance )
		if( weapon.hp )
			weapon.hp := weapon.hp - 1;
		endif
	endif

	if( armor.quality )
		if( (armor.hp > armor.maxhp) or (armor.hp < 1) )
			DestroyEquipment( defender, armor );
		endif
	endif

	if( weapon.quality )
		if( (weapon.hp > weapon.maxhp) or (weapon.hp < 1) )
			DestroyEquipment( attacker , weapon );
		endif
	endif

endfunction


function ModByProt( defender, basedamage )

	var physicalprot := GetObjProperty( defender , "PhysicalProtection" );
	if( !physicalprot )
		return basedamage;
	else
		basedamage := basedamage * ( 1 - physicalprot * 0.05 );
		return basedamage;
	endif

endfunction


function GetShieldAbsorption( attacker, defender, weapon, basedamage )

	var shield := GetEquipmentByLayer( defender , LAYER_HAND2 );
	var shieldar := CInt(shield.ar);
	if( shieldar )
		var parry_skill := GetEffectiveSkill( defender , SKILLID_BATTLE_DEFENSE );
		parry_skill := CInt( parry_skill * ClasseBonus( attacker , CLASSEID_WARRIOR ) );

		if( (Random(100) + 1) <= CInt(parry_skill / 2) )
			var absorbed;

			PerformAction( defender , ANIM_ATTACK_XBOW );
			SendSysmessage( defender , "You successfully parry the attack." );

			if( weapon.skillid == SKILLID_ARCHERY )
				absorbed := shieldar;
			else
				absorbed := CInt(shieldar / 2);
			endif

			basedamage := basedamage - absorbed;
			if( basedamage < 0 )
				basedamage := 0;
			endif

			MaceHit( attacker , defender , weapon , shield , absorbed );
		endif
	endif

	return basedamage;

endfunction


function DealDamage( attacker, defender, weapon, armor, basedamage, rawdamage )

	if( !GetObjProperty( weapon, "Astral" ) )

		var poisoned := GetObjProperty( weapon   , "SkillPoisoned" );
		var reactive := GetObjProperty( defender , "ReactiveArmor" );
		var onhit    := GetObjProperty( armor    , "OnHitScript" );

		//Check if the item was poisonned by someone
		if( poisoned )

			var plvl := CInt( poisoned / 3 );
			poisoned  := poisoned - 1;
			if( poisoned )
				SetObjProperty( weapon , "SkillPoisoned" , poisoned );
			else
				EraseObjProperty( weapon , "SkillPoisoned" );
			endif

			SetPoison( defender, plvl, "You have been poisoned by " + attacker.name + "." );

		endif

		if( GetObjProperty( defender , CLASSEID_WARRIOR ) )
			if( attacker.npctemplate )
				rawdamage := CInt( rawdamage / ClasseBonus( defender , CLASSEID_WARRIOR ) );
			else
				var level := GetObjProperty( defender , CLASSEID_WARRIOR );
				if( level )
					level := level - 1;
					if( level >= 1 )
						basedamage := CInt( basedamage * ClasseBonusByLevel( level ) );
					endif
				endif
			endif
		endif

		//Check if reactive armor spell is on
		if( reactive )
			start_script( ":combat:reactivearmoronhit" , { attacker, defender, basedamage, rawdamage, reactive} );
		endif

		//Check if the armor have an on hit script and launch it
		if( onhit )
			start_script( onhit , {attacker, defender, weapon, armor, basedamage, rawdamage} );
		else
			ApplyTheDamage( defender , attacker , rawdamage );
		endif

	else

		ApplyTheAstralDamage( defender , attacker , rawdamage );

	endif
DisplayDamage(attacker, defender, rawdamage);
var warstats := getobjproperty(attacker, "warstats");
warstats[1] := warstats[1] + rawdamage;
warstats[2] := warstats[2] + 1;
setobjproperty(attacker, "warstats", warstats);
endfunction


function DestroyEquipment( who, item )

	var cursed := GetObjProperty( item , "Cursed" );

	if( cursed )
		SetObjProperty( item , "Cursed" , 3 );
	endif

	if( who.backpack )
		MoveItemToContainer( item , who.backpack );
	endif

	if( who.npctemplate )
		if( item.skillid )
			PrintTextAbove( who , who.name + " seem exhausted and can barely hit anyones anymore!" );
		else
			PrintTextAbove( who , who.name + "'s defences has been penetrated making it easy to hit vital parts!" );
		endif
	endif

	DestroyItem( item );

endfunction


function CheckForTheSkillGain( attacker, defender, weapon )

	set_critical(1);

	if( !attacker.npctemplate )
		var weapon_skillid := weapon.skillid;
		var attacker_skills_state := GetObjProperty( attacker , "SkillsState" );
		if( !attacker_skills_state )
			attacker_skills_state := {};
		endif

		var attacker_tactics_state := attacker_skills_state[SKILLID_TACTICS+1];
		var attacker_weapon_state := attacker_skills_state[weapon_skillid+1];
		var attacker_wrestling_state := attacker_skills_state[SKILLID_WRESTLING+1];

		if( attacker_tactics_state == "l" or attacker_tactics_state == "d" )
			RemoveSkillGain( attacker , SKILLID_TACTICS , attacker_tactics_state );
		endif

		if( attacker_weapon_state == "l" or attacker_weapon_state == "d" )
			RemoveSkillGain( attacker , weapon_skillid , attacker_weapon_state );
		endif

		if( attacker_wrestling_state == "l" or attacker_wrestling_state == "d" )
			RemoveSkillGain( attacker , SKILLID_WRESTLING , attacker_wrestling_state );
		endif
	endif

	if( !defender.npctemplate )
		var defender_skills_state := GetObjProperty( defender , "SkillsState" );
		if( !defender_skills_state )
			defender_skills_state := {};
		endif

		var defender_wrestling_state	:= defender_skills_state[SKILLID_WRESTLING+1];
		var defender_swords_state	:= defender_skills_state[SKILLID_SWORDSMANSHIP+1];
		var defender_fencing_state	:= defender_skills_state[SKILLID_FENCING+1];
		var defender_mace_state	:= defender_skills_state[SKILLID_MACEFIGHTING+1];
		var defender_archery_state	:= defender_skills_state[SKILLID_ARCHERY+1];
		var defender_tactics_state	:= defender_skills_state[SKILLID_TACTICS+1];
		var defender_parry_state	:= defender_skills_state[SKILLID_PARRY+1];

		if( defender_wrestling_state == "l" or defender_wrestling_state == "d" )
			RemoveSkillGain( defender , SKILLID_WRESTLING , defender_wrestling_state );
		endif

		if( defender_swords_state == "l" or defender_swords_state == "d" )
			RemoveSkillGain( defender , SKILLID_SWORDSMANSHIP , defender_swords_state );
		endif

		if( defender_fencing_state == "l" or defender_fencing_state == "d" )
			RemoveSkillGain( defender , SKILLID_FENCING , defender_fencing_state );
		endif

		if( defender_mace_state == "l" or defender_mace_state == "d" )
			RemoveSkillGain( defender , SKILLID_MACEFIGHTING , defender_mace_state );
		endif

		if( defender_archery_state == "l" or defender_archery_state == "d" )
			RemoveSkillGain( defender , SKILLID_ARCHERY , defender_archery_state );
		endif

		if( defender_tactics_state == "l" or defender_tactics_state == "d" )
			RemoveSkillGain( defender , SKILLID_TACTICS , defender_tactics_state );
		endif

		if( defender_parry_state == "l" or defender_parry_state == "d" )
			RemoveSkillGain( defender , SKILLID_PARRY , defender_parry_state );
		endif
	endif

	set_critical(0);

endfunction


function RemoveSkillGain( who, skillid, state )

	set_critical(1);

	var combat_skills := {
		SKILLID_PARRY,
		SKILLID_TACTICS,
		SKILLID_ARCHERY,
		SKILLID_SWORDSMANSHIP,
		SKILLID_MACEFIGHTING,
		SKILLID_FENCING,
		SKILLID_WRESTLING
	};

	var i;
	for( i := Len( combat_skills ) ; i > 0 ; i := i - 1 )
		if( combat_skills[i] = skillid )
			break;
		endif
	endfor

	if( i > 0 )
		set_critical(0);
		return;
	endif

	var combat_level := GetObjProperty( who , "CombatSkillsLevel" );
	if( !combat_level )
		combat_level := {};
	endif

	var rawskill := GetRawSkill( who , skillid );
	foreach item in ListEquippedItems( who )
		if( GetObjProperty( item , "skilladv" ) = skillid )
			rawskill := rawskill - CInt( GetObjProperty( who , "advamount" + skillid + "s" + item.serial ) );
		endif
	endforeach

	var old_skill_level	:= CInt( combat_level[i] );
	var current_level	:= CInt( rawskill );

	if( state = "l" )

		if( current_level > old_skill_level )
			SetRawSkill( who , skillid , old_skill_level );
		endif

	elseif( state = "d" )

		var amount := 10000;
		if( amount >= old_skill_level )
			amount := old_skill_level;
			var state_array := GetObjProperty( who , "SkillsState" );
			if( !state_array )
				state_array := {};
			endif
			state_array[skillid+1] := "l";
			SetObjProperty( who , "SkillsState" , state_array );
		endif

		var new_level := CInt( old_skill_level - amount );
		SetRawSkill( who , skillid , new_level );
		combat_level[i] := new_level;
		SetObjProperty( who , "CombatSkillsLevel" , combat_level );

	endif

	set_critical(0);

endfunction


function DisplayDamage(attacker, damaged, dmg)
var atstat := getobjproperty(attacker, "showstats");
var damstat := getobjproperty(damaged, "showstats");
  if(dmg >= 255)
  dmg := 255;
  endif
    var pkt := "BF000B002201";
    pkt := pkt + fixPacketLength( (Hex(damaged.serial)-"0x"), 4) ;
    pkt := pkt + fixPacketLength( (Hex(dmg)-"0x"), 1);
  if(attacker.acct && atstat[8] == 1)
    SendPacket(attacker, pkt);
  endif
  if(damaged.acct && damstat[8] == 1)
    SendPacket(damaged, pkt);
  endif
endfunction

///////////////////////////////////////////////////////////////
//Pads a string with a "0" prefix, until it is of proper length
//  packetString   value to pad.
//  byteLength     number of bytes. Byte counted as two chars.
///////////////////////////////////////////////////////////////
function fixPacketLength( packetString, byteLength )
    if( !packetString || packetString == error )
        packetString := 0;
    endif
    packetString := cStr(packetString);

    if( packetString[1,2] == "0x" )
        packetString := cStr(packetString[3,len(packetString)]);
    endif

    if( len(packetString) > byteLength * 2 )
//      var extraBytes := (byteLength*2) - len(packetString);
      var extraBytes := len(packetString) - (byteLength*2) + 1;
      return cStr(packetString[extraBytes,len(packetString)]);
    endif

    while( len(packetString) < byteLength * 2 )
        packetString := "0" + packetString;
    endwhile

    return packetString;
endfunction
