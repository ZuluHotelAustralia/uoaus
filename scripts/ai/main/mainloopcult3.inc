function main_AI_loop()

    var ev;
    var wanders := 60;
    var next_wander := ReadGameClock();
    var next_news := ReadGameClock();
    EnableMainEvents();

	var mydestx := 0;
	var mydesty := 0;
	var steps := 0;

	SetAnchor( CInt(me.x), CInt(me.y), 4, 50 );

    while (1)

	if (ReadGameClock() > next_wander)
//		if (readgameclock() > next_news)
//			SpoutTheNews();
//			next_news := ReadGameClock()+60;
//		endif

		if (mydestx != 0)
			WalkTowardLocation(mydestx, mydesty);
			if ( (me.x == mydestx && me.y == mydesty) || ( steps > 10 ) )
				mydestx := 0;
			endif
		else
			wander();
			wanders := wanders +1;
		endif

		next_wander := ReadGameClock()+5;
		if (wanders > 60)
			wanders :=0;
			walkhome();
			ev := sleepmode();
    		endif
	else
		ev := os::wait_for_event( 2 );
	endif

	if (ev)
        repeat
            case (ev.type)
                SYSEVENT_SPEECH:
		    if (!ev.source.isA(POLCLASS_NPC))
		    	if (lower(ev.text) == "news")
				wanders :=0;
		    		next_wander := ReadGameClock()+60;
		    		SpoutTheNews();
		    	endif
		    endif
                SYSEVENT_ENGAGED:
                SYSEVENT_DAMAGED:
                    Fight( ev.source );
		    ReturnHome();
		EVID_ALL_FOLLOW_CMD:
			mydestx := CInt(ev.x);
			mydesty := CInt(ev.y);
			steps := 0;
			SetAnchor( mydestx, mydesty, 4, 50 );
			WalkTowardLocation(mydestx, mydesty);
		SYSEVENT_ENTEREDAREA:
		saynews:=1;
		SpoutTheNews();
		SYSEVENT_LEFTAREA:
		saynews:=0;
            endcase

        until (! (ev := os::wait_for_event(2)) );
	endif

   endwhile     

endfunction       

function look_around()

	return;

endfunction

function EnableMainEvents()

    DisableEvents( SYSEVENT_LEFTAREA + SYSEVENT_DISENGAGED + SYSEVENT_OPPONENT_MOVED );
    EnableEvents( SYSEVENT_SPEECH + SYSEVENT_ENGAGED + SYSEVENT_DAMAGED, HALT_THRESHOLD );
    EnableEvents( SYSEVENT_SPEECH, 3 );
    DisableEvents(SYSEVENT_ITEM_GIVEN);

endfunction

function DisableMainEvents()

    DisableEvents( SYSEVENT_SPEECH + SYSEVENT_ENGAGED + SYSEVENT_DAMAGED );
    DisableEvents(SYSEVENT_ITEM_GIVEN);

endfunction

function SpoutTheNews()
say("These 'Watchers' do not sound like good fellows at all.");
sleep(4);
say("A new world order? Sounds like treason!");
sleep(4);
say("'Unseen One?' Sounds like a daemon of some sort...");
sleep(4);
say("Why won’t they listen to reason! Verily this is a cult...");
sleep(4);
say("We cannot heal the minds of those who do not wish to be healed.");
sleep(4);
say("I sense much evil and danger in the 'Watchers' rants...");
sleep(4);
say("I have heard of their ritual of Sacrifice and it sounds dangerous...");
sleep(4);
say("Truly they must be madmen.");
sleep(15);
endfunction

function ReturnHome()

	var myhome := GetObjProperty(me,"myhome");
	MoveCharacterToLocation(me,myhome[1],myhome[2],myhome[3],MOVECHAR_FORCELOCATION);

endfunction
