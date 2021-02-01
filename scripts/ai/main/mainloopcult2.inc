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
say("There will be a new world order! Will you be apart of it?");
sleep(3);
say("The chosen one has the 'Great Plan' that involves you!");
sleep(3);
say("Evil runs rampant while the innocent suffer, Lord British does nothing!");
sleep(3);
say("The 'Priests' of this land worship False Idols!");
sleep(3);
say("The 'Unseen One' shall roam this earth once again!");
sleep(3);
say("The 'Unseen One' is the True Power to be worshipped!");
sleep(3);
say("The 'Unseen One' will bring a great time of peace!");
sleep(3);
say("It is the False Idols who are to blame for your sufferings!");
sleep(3);
say("The so called Priests and Sages of this land do not want you to know the 'Unseen One'");
sleep(3);
say("The 'Unseen One' wants you to share his power!");
sleep(3);
say("The end is nigh without knowing the 'Unseen One'!");
sleep(15);

endfunction

function ReturnHome()

	var myhome := GetObjProperty(me,"myhome");
	MoveCharacterToLocation(me,myhome[1],myhome[2],myhome[3],MOVECHAR_FORCELOCATION);

endfunction
