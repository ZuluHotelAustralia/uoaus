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
if (ev.text["Quest"] || ev.text["quest"])
   PrintTextAbovePrivate( me, "Lord British has asked me to undertake the role of investigating this *Cult*. I have been empowered to enlist the aide of Britain Citizens to help me.", ev.source );

			elseif (ev.text["Cult"])
   PrintTextAbovePrivate( me, "Yes, a troubling scenario has developed indeed. Strange folk have started preaching about this Cult in Yew. The problem is we dont know much about these *Watchers*", ev.source );
			elseif (ev.text["Watchers"])
   PrintTextAbovePrivate( me, "Yes, the strange folk. Perhaps a task for you could be to discover more about them, and return and tell about their leaders or rituals. I want to know what they are up to. Good Luck, citizen.", ev.source );
			elseif (ev.text["Unseen One"])
   PrintTextAbovePrivate( me, "Unseen One? Hrmm, sounds like no Diety i know, perhaps these people have fallen under the sway of some Daemon Lords influence. Did they mention anything about rituals or *sacrifice*?", ev.source );
			elseif (ev.text["Sacrifice"])
   PrintTextAbovePrivate( me, "So they have mentioned Sacrifice as part of a ritual? Yes, this is indeed the work of Evil. However we will need concrete evidence. I will diliberate with my superiors on our next course of action. I thank thee for thy time, brave Citizen.", ev.source );
		    	endif
		    endif
      sleep(5);
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

say("Troubling times ahead...");
sleep(2);
say("What shall i do about this?");
sleep(2);
say("Perhaps we could arrest them all and throw them in jail!");
sleep(2);
say("Bunch of Lunatics!");
sleep(2);
say("They shall feel my my blade if a citizen is hurt!");

endfunction

function ReturnHome()

	var myhome := GetObjProperty(me,"myhome");
	MoveCharacterToLocation(me,myhome[1],myhome[2],myhome[3],MOVECHAR_FORCELOCATION);

endfunction
