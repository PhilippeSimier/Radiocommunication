/* 
   VHF TNC DRA818 APRS Arduino library
   Created 23/2/2022
   Anthony LE CREN f4goh@orange.fr
   Modified
   Use this library freely
   AX25 RX routines from  Robert Marshall, KI4MCW and 
   Dennis Seguine http://www.cypress.com/?docID=2328
 */

#include "Tnc.h"

Tnc::Tnc() : 
dra(new DRA818()) 
{
  anchor = this;  
}

Tnc::Tnc(const Tnc& orig) {
}

Tnc::~Tnc() {
}

void Tnc::begin(int p_led) {    
    led = p_led;
    pinMode(led, OUTPUT);
    timer = timerBegin(0, 80, true);
    timerAttachInterrupt(timer, Tnc::marshall, true);
    timerAlarmWrite(timer, 76, true);
    timerAlarmEnable(timer);
    digiTX = 0;
    adc1_config_width(ADC_WIDTH_BIT_9); //9 bits
    adc1_config_channel_atten(PIN_ADC, ADC_ATTEN_DB_11);
    //adcAttachPin(36);
    uint8_t ret;
    //modelDra, freq_rx, freq_tx, squelch, vol, ctcss_rx, ctcss_tx, bandwidth, pre, high, low

    ret = dra->configure(DRA_TYPE, TX_RX_FREQUENCY, TX_RX_FREQUENCY, 4, 8, 0, 0, DRA818_12K5, false, false, false);
    if (!ret) {
        Serial.println("\nError while configuring DRA818");
    } else {
        Serial.println("DRA818 configuration finished");
    }
}


void Tnc::marshall() {
    anchor->interruption();
}


/********************************************************
 * AX25 RX routines from  Robert Marshall, KI4MCW 
 ********************************************************/

void IRAM_ATTR Tnc::interruption()

{
    // heartbeat on led
    digitalWrite(led,digitalRead(led)^1);
    // calculate ADC bias (average of last 128 ADC samples)
    // this input is decoulped from the receiver with a capacitor, 
    // and is re-biased to half of the Arduino regulated +3.3V bus
    // with a voltage divider. therefore the net bias should equal
    // (more or less) the settle point of the voltage divider.
    // doing this in software also means that the calculated bias
    // will re-center automatically if the resistors are not 
    // perfectly matched, etc.
    
    rawadc = adc1_get_raw(PIN_ADC);    
    bias_sum += rawadc ;
    if ( ++bias_cnt == 128 )
    {
        adc_bias = bias_sum >> 7 ;
        bias_cnt = 0 ;
        bias_sum = 0 ;
    }
    
   
    //=========================================================
    // Seguine math
    //    for details, see http://www.cypress.com/?docID=2328
    //=========================================================


    adcval = rawadc - adc_bias ;
	
    // circle buffer is just 7 elements (about half of a bit duration)
    if (++cb_pos == 7) { cb_pos = 0 ; }
	
    // back out old value from sum
    mult_sum -= mult_cb[ cb_pos ] ;
	
    // multiply the latest ADC value by the value ~1/2 lo-freq duration ago. if the 
    // incoming audio is the 1200 Hz MARK tone, the two samples will (usually) have 
    // opposite phases/signs, so multiplying the two values will give a negative result. 
    // If the incoming audio is 2200 Hz, the two samples usu. have the same phase/sign, 
    // so multiplying them will give a positve result.
    // subscript 5 = six samples ago-------v
    mult_cb[ cb_pos ] = adcval * adc_delay[5] ;
	
    // add this result to get the sum of the last 7 multipliers (1st LPF)
    mult_sum += mult_cb[ cb_pos ] ;
	
    // force a definitive answer with a hysteresis zone around zero (2nd LPF)
    if      ( mult_sum >=  100 ) { current_symbol = AX25_SPACE ; }
    else if ( mult_sum <= -100 ) { current_symbol = AX25_MARK ;  }
    else                         { ; } // inconclusive - dont change
		
    // increment # of samples since last symbol change, enforce a max
    if ( ++since_last_chg > 200 ) { since_last_chg = 200 ; }
    thesebits = 0 ;
	
    
    // rotate the delay
    for ( x=5 ; x>=1 ; x-- ) 
    {
        adc_delay[x] = adc_delay[x-1] ;
    }	
    adc_delay[0] = adcval ;

	
    //=============================
    //   clock and bit recovery
    //=============================

	
    // the in-a-frame and seeking-frame-start modes require different strategies
    // let's split them up here

    if ( inaframe ) 
    {
        //================================
	// clock recovery within a frame
	//================================
		
	// check symbol state only once per bit time (3rd LPF)
	bit_timer-- ;
		
	if ( current_symbol != last_symbol ) 
	{
	    // save the new symbol
	    last_symbol = current_symbol ;
	    // reset counter
	    since_last_chg = 0 ;
			
	    // Ideally, frequency transitions will occur on a since-last-change 
	    // count that is an exact bit duration at the 1200 Hz signaling 
	    // rate - that is, an exact multiple of 11 samples (11 samples = 1 bit, 
	    // 22 = 2 bits, 33 = 3, etc). To give some extra settle time, we 
	    // don't attempt to read the symbol value at the exact moment of 
	    // transition; instead, we give it 4 extra beats. Thus as bit_timer 
	    // is counting down to the next symbol check, its value should ideally 
	    // be 4 when the symbol change actually takes place. If the symbol 
	    // change is a little early or late, we can tweak the bit_timer to
	    // tolerate some drift and still keep sync.
	    // By those rules, an SLC of 4 is perfect, 2 through 6 are fine and  
	    // need no adjustment, 7,8 and 0,1 need adjustment, 9,10 are timing 
            // errors - we can accept a certain number of those before aborting.
			
	    if ( ( bit_timer == 7 ) || ( bit_timer == 8 ) )
	    {
	        // other station is slow or we're fast. nudge timer back.
		bit_timer -= 1 ;
		
	    }
	    else if ( ( bit_timer == 0 ) || ( bit_timer == 1 ) )
	    {
	        // they're fast or we're slow - nudge timer forward
		bit_timer += 1 ;
		  }	
	    else if ( ( bit_timer == 9 ) || ( bit_timer == 10 ) )
	    {
	        // too much error
		if ( ++sync_err_cnt > MAX_SYNC_ERRS ) 
		{
		    sync_err_cnt = 0 ;
		    msg_pos = 0 ;
		    inaframe = 0 ;
		    bitq = 0 ;
		    bitqlen = 0 ;
		    bit_timer = 0 ;
		    bittimes = 0 ;
		    // turn off DCD light
		   // DCD_OFF ;
		    return ;
		}
	//	else
	//	{
		    		
	//	}
	    } // end bit_timer cases
	} // end if symbol change
		
        //=============================
	// bit recovery within a frame
	//=============================
		
	if ( bit_timer == 0 )
	{
            // time to check for new bits
	    
	  		
	    // reset timer for the next bit
	    bit_timer = 11 ;
	    // another bit time has elapsed
	    bittimes++ ;
			
	    // wait for a symbol change decide what bits to clock in,
            // and how many
            if ( current_symbol != last_symbol_inaframe ) 
	    { 
	        // add one as ready-to-decode flag
		thesebits = bittimes + 1 ; 
		bittimes = 0 ;
		last_symbol_inaframe = current_symbol ;
            }
            
	} // end if bit_timer==0 

    } // end if inaframe

    else
    {
        //=================
	// not in a frame
	//=================
		
	// housekeeping
	// phase_err = since_last_change MOD 11, except that the "%" operator is =very slow=
	phase_err = since_last_chg ;
	while ( phase_err >= 11 ) { phase_err -= 11 ; }
	
	// elapsed bittimes = round (since_last_chg / 11)
	bittimes = 0 ;
	test = since_last_chg + 5 ;
	while ( test > 11 ) { test -= 11 ; bittimes++ ; }
	thesebits = 0 ;
	
	//====================================
	// clock recovery NOT within a frame
	//====================================
		
	// our bit clock is not synced yet, so we will need a symbol transition 
        // to either establish sync or to clock in bits (no transition? exit ISR)
	if ( current_symbol == last_symbol ) 
	{ return ; }
	
        // symbol change 

	// save the new symbol, reset counter
	last_symbol = current_symbol ;
	since_last_chg = 0 ;
	
	// check bit sync
	if ( ( phase_err >= 4 ) && ( phase_err <= 7 ) )
	{
	    // too much error
	    bitq = 0 ;
	    bitqlen = 0 ;
            // turn off the DCD light
           // DCD_OFF ;
	}	
		
	// save these bits
	thesebits = bittimes + 1 ;
			
    } // end else ( = not inaframe)
	
	
    //========================================
    //   bit recovery, in or out of a frame
    //========================================
	

    // if no bit times have elapsed, nothing to do
    if ( thesebits == 0 ) { return ; }
    else                  { thesebits-- ; }  // remove the "ready" flag
	
    // determine incoming bit values based on how many bit times have elapsed.
    // whatever the count was, the last bit involved a symbol change, so must be zero.
    // all other elapsed bits must be ones. AX.25 is transmitted LSB first, so in
    // adding bits to the incoming bit queue, we add them right-to-left (ie, new bits
    // go on the left). this lets us ready incoming bytes directly from the lowest
    // eight bits of the bit queue (once we have that many bits).
    
    // the act of adding bits to the queue is in two parts - (a) OR in any one bits,
    // shifting them to the left as required prior to the OR operation, and (b) update
    // the number of bits stored in the queue. with zero bits, there's nothing to
    // OR into place, so they are taken care of when we update the queue length, and 
    // when we shift the queue to the right as bytes are popped off the end later on.
    
    switch ( thesebits )
    {
    case 1: break ;    // no ones to add ------> binary       "0"
                            
    case 2: bitq |= ( 0x01 << bitqlen ) ;     // binary      "01"
            break ;
                           
    case 3: bitq |= ( 0x03 << bitqlen ) ;     // binary     "011"
            break ;

    case 4: bitq |= ( 0x07 << bitqlen ) ;     // binary    "0111"
            break ;
                           
    case 5: bitq |= ( 0x0F << bitqlen ) ;     // binary   "01111"
            break ;
                           
    // "six" is a special case ("bitstuff"): drop the zero, and only add the 5 one bits
    case 6: bitq |= ( 0x1F << bitqlen ) ;     // binary   "11111"
            thesebits = 5 ;
            break ;
                  
    // "seven" is another special case - only legal for an HDLC byte			  
    case 7:                                   // binary "0111111"
            if ( ( bitqlen == 1 ) && ( bitq == 0 ) )
	    {
	        // only one bit pending, and it's a zero 
		// this is the ideal situation to receive a "seven".
		// complete the HDLC byte
		bitq = 0x7E ;
		bitqlen = 8 ;
            }
            else if ( bitqlen < 4 )
            {
                // 0-3 bits still pending, but not the ideal single-zero.
		// this is kinda ugly. let's dump whatever is pending, 
		// and close the frame with a tidy right-justified HDLC.
		bitq = 0x7E ;
		bitqlen = 8 ;
            }				
            else if ( bitqlen >= 4 )
            {
                // also kinda ugly - half or more of an unfinished byte.
                // lets hang onto the pending bits by fill out this 
                // unfinished byte with zeros, and append the full HDLC 
                // char, so that we can close the frame neatly.
		bitq = ( bitq & 0xFF ) | 0x7E00 ;
		bitqlen = 16 ;
            }
            else 
            {
                // huh?!? ok, well, let's clean up
		bitq = 0x7E ;
		bitqlen = 8 ;
            }
            
            // we've already made the necessary adjustments to bitqlen,
            // so do not add anything else (below)
            thesebits = 0 ;
	    break ;
  
    default: 
            // less than a full bit, or more than seven have elapsed
            // clear buffers
            
            msg_pos = 0 ;
            inaframe = 0 ;
            bitq = 0 ;
            bitqlen = 0 ;
            // do not add to bitqlen (below)
            thesebits = 0 ;
            // turn off DCD light
          //  DCD_OFF ;
            break ;
                   
    } // end switch

    // how many bits did we add?
    bitqlen += thesebits ;
              
	
    //===================
    //   byte recovery
    //===================


    // got our bits in a row. now let's talk about bytes.

    // check the bit queue, more than once if necessary
    while ( bitqlen >= 8 )
    {
        // take the bottom 8 bits to make a byte
        byteval = bitq & 0xFF ;

        // special case - HDLC frame marker
        if ( byteval == 0x7E )
        {
            if ( inaframe == 0 ) 
            {
                // marks start of a new frame               
                inaframe = 1 ;
                last_symbol_inaframe = current_symbol ;
		sync_err_cnt = 0 ;
		bit_timer = 15 ;
		bittimes = 0 ;
                // pop entire byte (later)
                popbits = 8 ;
            }	

            else if ( msg_pos < MIN_PACKET_LEN )
            {
                // We are already in a frame, but have not rec'd any/enough data yet.
                // AX.25 preamble is sometimes a series of HDLCs in a row, so 
                // let's assume that's what this is, and just drop this byte.
		
                popbits = 8 ;
            }    
           
            else     
            {
                // in a frame, and have some data, so this HDLC is probably 
                // a frame-ender (and maybe also a starter)
				
                if ( msg_pos > 0 )
                {   /*
                    printf( "Message was:" ) ;
                    for ( x=0 ; x < msg_pos ; x++ )
                    {
                        printf( " %02X", msg[x] ) ;
                    }    
                    printf( "\n" ) ; 
                    printf( "Which decodes as:\n" ) ;
                    */
                    // send frame data out the serial port
                    decode_ax25() ;
                }

                // stay in inaframe-mode, in case a new frame is starting
		msg_pos = 0 ;
		sync_err_cnt = 0 ;
		bittimes = 0 ;
		// pop entire byte (later)
                popbits = 8 ;
              
            }  // end else for "if not inaframe"

        }  // end if byteval = 0x7E

        else if ( inaframe == 1 ) 
        {
            // not an HDLC frame marker, but =is= a data character
            

            // add it to the incoming message
            msg[ msg_pos ] = byteval ;
            msg_pos++ ;
            
            // is this good enough of a KISS frame to turn on the carrier-detect light?
            // we know we have an HDLC (because we're in a frame); 
            // check for end-of-header marker
          //  if ( byteval == 0x03 ) { DCD_ON ; }
            
            // pop entire byte (later)
            popbits = 8 ;
        }    

        else
        { 
            // not already in a frame, and this byte is not a frame marker.
            // It is possible (likely) that when an HDLC byte arrives, its 8 bits will not 
            // align perfectly with the 8 we just checked. So instead of dropping all 8 of 
            // these random bits, let's just drop one, and re-check the rest again later. 
            // This increases our chances of seeing the HDLC byte in the incoming bitstream
            // amid noise (esp. if we're running open squelch).
            popbits = 1 ; 
        }

        // pop the used bits off the end 
        // (i know, not really a "pop", more of a "drop")
        bitq = ( bitq >> popbits ) ;
        bitqlen -= popbits ;

    } // end while bitqueue >= 8

   
}  // end timer interrupt




void Tnc::decode_ax25 (void)
{
  // Take the data in the msg array, and send it out the serial port protocol KISS
  // https://en.wikipedia.org/wiki/KISS_(TNC)

  x = 0 ;
  decode_state = 0 ;     // 0=just starting, 1=header, 2=got 0x03, 3=got 0xF0 (payload data)

  //*************solution actuelle
  // lop off last 2 bytes (FCS checksum, which we're not sending to the PC)
  for ( x = 0 ; x < (msg_pos - 2) ; x++ )
  {
    switch ( decode_state )  
    {
      // note the goofy order!!
    case 0:  
      // just starting

      Serial.write(0xC0);                 // frame start/end marker

      Serial.write(0x00);                 // data on port 0
      Serial.write(msg[x]);

      decode_state = 1 ;
      break ;

    case 2: 
      // got the 0x03, waiting for the 0xF0
      if ( msg[x] == 0xF0 ) 
      { 

        Serial.write(msg[x]);

        decode_state = 3 ;
      }
      else 
      {
        // wow - corrupt packet? abort

        // Serial.write(13); 
        // Serial.write(10); 
        return ;
      } 
      break ;

    case 1: 
      // in the header
      if ( msg[x] == 0x03 ) 
      { 

        Serial.write(msg[x]);

        decode_state = 2 ; 
        break ;
      }
      // else fall through

    default:
      // payload or header
      if ( msg[x] == 0xC0 ) { 
        Serial.write(0xDB) ;
      }
      Serial.write(msg[x]);
      if ( msg[x] == 0xDB ) { 
        Serial.write(0xDD) ;
      }
      break ;
    }  
    
  } // end for  
  Serial.write(0xC0);  // end of frame
}

Tnc* Tnc::anchor = NULL;
 
