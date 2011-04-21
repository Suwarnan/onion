/*********************************************************************
 * Copyright (c) 2011 Jan Pomikalek                                  *
 * All rights reserved.                                              *
 *                                                                   *
 * This software is licensed as described in the file COPYING, which *
 * you should have received as part of this distribution.            *
 *********************************************************************/

#include "buzhash.h"
#include <string.h>
#include <stdlib.h>

const hash_t CHAR2LONG[255] = {
    12658332951230890439ul,
    16607337219466274820ul,
    4897781435750669512ul,
    1863954398247708433ul,
    6041299601906237138ul,
    3602934247356726349ul,
    13927570682514441143ul,
    11920701378039577834ul,
    14629533900929623503ul,
    16546862913458629335ul,
    10685855460932754325ul,
    15186106020611570871ul,
    8131473594228677807ul,
    9287569521752445451ul,
    5624316205208212365ul,
    10693223548395698341ul,
    10578473704599778022ul,
    16693921798782755893ul,
    15124492184888274523ul,
    1235529881146962610ul,
    14843219789508576687ul,
    15526012670070475388ul,
    6463116610490435782ul,
    15104307767477900194ul,
    8484741665705462025ul,
    8100868536101218192ul,
    3395269876321120613ul,
    8589680476807032865ul,
    7621819336684948355ul,
    14153065448097834589ul,
    6732762317790231782ul,
    13018363043978374122ul,
    6215216690161075437ul,
    9357943660640904950ul,
    12116224851753945911ul,
    13636661669728066501ul,
    7484247892091601413ul,
    8512193125891820287ul,
    10461835854496665155ul,
    16797036920317134766ul,
    1313270971513831546ul,
    742840173802188917ul,
    1249430170856643161ul,
    17179028999057074571ul,
    6378210156955744140ul,
    793680677819467304ul,
    4263415984887072454ul,
    7875662396850393478ul,
    17050561532048146107ul,
    1435098142595853720ul,
    8580942225888237636ul,
    13308656650323976644ul,
    16630067181906003651ul,
    12116795942522001627ul,
    9892291673171748547ul,
    11660673438127243284ul,
    6026050291617469826ul,
    10478522635079777192ul,
    12138158317934008218ul,
    3518644136578100667ul,
    4950215611630576830ul,
    15769242181285477405ul,
    7950690203065752077ul,
    319974224259159447ul,
    9604177767109474443ul,
    2499971183666009670ul,
    3389512945436469180ul,
    13643083464485449791ul,
    7197237438818751483ul,
    11151212581995191915ul,
    17495196072216154799ul,
    6770497232845758508ul,
    10987981514044724191ul,
    14707120191905416074ul,
    1769092362238593010ul,
    9329650998411009452ul,
    14719126903328637772ul,
    16952770464905740286ul,
    9674713352706546441ul,
    6649376341374010415ul,
    13209384319143003802ul,
    15927169943220646170ul,
    16897589646525214220ul,
    3262252579774962994ul,
    12644188031911778084ul,
    12242729612781990566ul,
    10411593575032306840ul,
    6901591497302664456ul,
    16282753866514979972ul,
    1656537748780076590ul,
    16482447327676653424ul,
    15257560081058078415ul,
    2959473391892618753ul,
    6837204821782891114ul,
    10938562237399133186ul,
    16857781777840528196ul,
    8483325299592247627ul,
    8376541859638180551ul,
    2504977066327782390ul,
    12231409223811250404ul,
    4744310199064570243ul,
    17936677873798959622ul,
    7126990633455442871ul,
    2079219814712678870ul,
    5067179041865164597ul,
    2311488369720591961ul,
    1725854410047761352ul,
    7355938747639265690ul,
    15490596914355917847ul,
    2283460595124192686ul,
    6878856348493276219ul,
    9152647736939983958ul,
    1662432522495537695ul,
    11306127178924536002ul,
    9272318044070549747ul,
    7145744474881723964ul,
    13448381548771200536ul,
    16160887140379377718ul,
    16369357319459660843ul,
    5476117262347077406ul,
    16602075379238506563ul,
    11456607228896734049ul,
    6465411526782391145ul,
    8155612729101736593ul,
    1740403063688953650ul,
    4466509242016709213ul,
    18112502299939680520ul,
    16974090059556845575ul,
    12326512096507303015ul,
    15376655537080530798ul,
    12498441914565269305ul,
    6036826437421754258ul,
    7912527257991934972ul,
    16620739722007677741ul,
    8733477150731820655ul,
    16564684276929490022ul,
    9409261669616170022ul,
    8387885649776441101ul,
    4427301691848253832ul,
    17640389513959398145ul,
    11987577927023442578ul,
    1358867256273478740ul,
    14172638869615591470ul,
    4669134809929205329ul,
    9146890779639199412ul,
    2448139160410716046ul,
    14539456923687813097ul,
    15701779011641704372ul,
    12184110908386419117ul,
    6182072944631238310ul,
    6068503614243670324ul,
    17486237705261861510ul,
    8141926135459860042ul,
    11247558917664640122ul,
    15966973352605162329ul,
    9274584296089522436ul,
    16106837601580129961ul,
    5565067011055473713ul,
    9018591362895332601ul,
    17429669259725580644ul,
    5862130260298638241ul,
    10804107644379464482ul,
    14590678293851680311ul,
    7586397638435564357ul,
    5024282990565981028ul,
    17710866669113912150ul,
    10607302159042519593ul,
    10224690187282473862ul,
    12691341730791771243ul,
    446919220230245087ul,
    11928822690215012312ul,
    14695552131553031715ul,
    9373710656266261295ul,
    10535666776941439244ul,
    4764286487123496201ul,
    12081558227095427560ul,
    14657526787837780677ul,
    4854775944749701021ul,
    18014893051074447624ul,
    5961551484053396826ul,
    7007393494224833114ul,
    1918625258470397717ul,
    2249596653018019968ul,
    15376752853428300944ul,
    15661589396388907215ul,
    17959491169395034186ul,
    7412669116831624121ul,
    16613322186307011607ul,
    1168394068192978862ul,
    13541384245715877822ul,
    17842264847294623193ul,
    8656129051250713732ul,
    6600363660893585591ul,
    10437456264051898071ul,
    6483876479559582910ul,
    2351460095187333222ul,
    17709647483310915437ul,
    4687819186773626811ul,
    12859142186029646747ul,
    14196439022719216916ul,
    10831194418958921226ul,
    9958754500157295475ul,
    2812703802823563549ul,
    364639487745161427ul,
    18071223067394944401ul,
    11148005916176784196ul,
    10887057658503987840ul,
    7239832157577921295ul,
    6274798767279704963ul,
    9654930315473449062ul,
    11342083202968693359ul,
    8060885109403789727ul,
    532804797012507628ul,
    4259820420986796757ul,
    3591121934050292837ul,
    3739649723128072566ul,
    11338759925899470208ul,
    17557031182161531657ul,
    1328316363081986551ul,
    905104119772647733ul,
    16162805969666123858ul,
    13351191969939227039ul,
    11181921000405417530ul,
    1257129276560696939ul,
    8049492553042309720ul,
    8867122601488545729ul,
    8169023185794623188ul,
    14027174324336484013ul,
    3026556086188399794ul,
    7137339202398299406ul,
    15636400854018083176ul,
    1912758983363371197ul,
    12934014134659659938ul,
    6432162334519755563ul,
    11890239098696368321ul,
    465021739668949123ul,
    3571688800220472097ul,
    17356096479830501074ul,
    17244551474859817129ul,
    16858016682994011520ul,
    11599911656842386375ul,
    1384801604554958238ul,
    10350053496655489375ul,
    2028044935420165668ul,
    9321839731809955516ul,
    3800717409646038380ul,
    508616612214119935ul,
    14489270436014461891ul,
    11373150082561320490ul,
    8855221204049336307ul,
    11920562817555372746ul,
    17464569634060446109ul,
    146583913832133545ul,
    11454565731520647642ul,
    14516679283835536061ul,
};

// 64-bit left circular shift
hash_t rotate_left(hash_t value, int shift) {
    return (value << shift) | (value >> (64 - shift));
}

hash_t hash_string(char* string) {
    hash_t hash = 0;
    int string_len = strlen(string);
    int i;
    for (i=0; i<string_len; i++) {
        hash ^= CHAR2LONG[(unsigned char) string[i]];
        hash = rotate_left(hash, 1);
    }
    return hash;
}

void buzhash_init_buffer(buzhash_buffer_t* buffer, int size) {
    buffer->size = size;
    buffer->elem_count = 0;
    buffer->last_index = size - 1;
    buffer->hash = 0;
    buffer->elems = (hash_t*) malloc(size * sizeof(hash_t));
}

void buzhash_clear_buffer(buzhash_buffer_t* buffer) {
    buffer->elem_count = 0;
    buffer->last_index = buffer->size - 1;
    buffer->hash = 0;
}

void buzhash_free_buffer(buzhash_buffer_t* buffer) {
    free(buffer->elems);
}

int buzhash_is_full_buffer(buzhash_buffer_t* buffer) {
    return (buffer->elem_count == buffer->size);
}

hash_t buzhash(char* string, buzhash_buffer_t* buffer) {
    hash_t string_hash = hash_string(string);
    if (buffer->elem_count < buffer->size) {
        buffer->last_index = (buffer->last_index + 1) % buffer->size;
        buffer->hash = rotate_left(buffer->hash, 1) ^ string_hash;
        buffer->elems[buffer->last_index] = string_hash;
        buffer->elem_count++;
    }
    else {
        int fst_index = (buffer->last_index + 1) % buffer->size;
        buffer->hash = rotate_left(buffer->hash, 1) ^ rotate_left(
                buffer->elems[fst_index], buffer->size) ^ string_hash;
        buffer->last_index = fst_index;
        buffer->elems[buffer->last_index] = string_hash;
    }
    return buffer->hash;
}