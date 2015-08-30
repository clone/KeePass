ECHO OFF

DEL Bulgarian.lng
Util\CprLang.exe BulgarianSrc.lng Bulgarian.lng

DEL Czech.lng
Util\CprLang.exe CzechSrc.lng Czech.lng

DEL Danish.lng
Util\CprLang.exe DanishSrc.lng Danish.lng

DEL Dutch.lng
Util\CprLang.exe DutchSrc.lng Dutch.lng

DEL Estonian.lng
Util\CprLang.exe EstonianSrc.lng Estonian.lng

DEL French.lng
Util\CprLang.exe FrenchSrc.lng French.lng

DEL German.lng
Util\CprLang.exe GermanSrc.lng German.lng

DEL Greek.lng
Util\CprLang.exe GreekSrc.lng Greek.lng

DEL Hebrew.lng
Util\CprLang.exe HebrewSrc.lng Hebrew.lng

DEL Hungarian.lng
Util\CprLang.exe HungarianSrc.lng Hungarian.lng

DEL Italian.lng
Util\CprLang.exe ItalianSrc.lng Italian.lng

DEL Japanese.lng
Util\CprLang.exe JapaneseSrc.lng Japanese.lng

DEL Korean.lng
Util\CprLang.exe KoreanSrc.lng Korean.lng

DEL Norwegian.lng
Util\CprLang.exe NorwegianSrc.lng Norwegian.lng

DEL Polish.lng
Util\CprLang.exe PolishSrc.lng Polish.lng

DEL Portuguese.lng
Util\CprLang.exe PortugueseSrc.lng Portuguese.lng

DEL Russian.lng
Util\CprLang.exe RussianSrc.lng Russian.lng

DEL Slovak.lng
Util\CprLang.exe SlovakSrc.lng Slovak.lng

DEL Spanish.lng
Util\CprLang.exe SpanishSrc.lng Spanish.lng

DEL Swedish.lng
Util\CprLang.exe SwedishSrc.lng Swedish.lng

DEL Turkish.lng
Util\CprLang.exe TurkishSrc.lng Turkish.lng

DEL Macedonian.lng
Util\CprLang.exe MacedonianSrc.lng Macedonian.lng

DEL TraditionalChinese.lng
COPY TraditionalChineseSrc.lng TraditionalChinese.lng

DEL Ukrainian.lng
COPY UkrainianSrc.lng Ukrainian.lng

DEL SimplifiedChinese.lng
COPY SimplifiedChineseSrc.lng SimplifiedChinese.lng

ECHO.
ECHO Have you updated the language information fields?

REM Copy latest language files into Debug directory

DEL ..\Debug\German.lng
COPY /B German.lng /B ..\Debug\German.lng > NUL
DEL ..\Debug\French.lng
COPY /B French.lng /B ..\Debug\French.lng > NUL
DEL ..\Debug\Dutch.lng
COPY /B Dutch.lng /B ..\Debug\Dutch.lng > NUL
DEL ..\Debug\Swedish.lng
COPY /B Swedish.lng /B ..\Debug\Swedish.lng > NUL
DEL ..\Debug\Korean.lng
COPY /B Korean.lng /B ..\Debug\Korean.lng > NUL
DEL ..\Debug\Czech.lng
COPY /B Czech.lng /B ..\Debug\Czech.lng > NUL
DEL ..\Debug\Estonian.lng
COPY /B Estonian.lng /B ..\Debug\Estonian.lng > NUL
DEL ..\Debug\Portuguese.lng
COPY /B Portuguese.lng /B ..\Debug\Portuguese.lng > NUL
DEL ..\Debug\Bulgarian.lng
COPY /B Bulgarian.lng /B ..\Debug\Bulgarian.lng > NUL
DEL ..\Debug\Slovak.lng
COPY /B Slovak.lng /B ..\Debug\Slovak.lng > NUL
DEL ..\Debug\Norwegian.lng
COPY /B Norwegian.lng /B ..\Debug\Norwegian.lng > NUL
DEL ..\Debug\Polish.lng
COPY /B Polish.lng /B ..\Debug\Polish.lng > NUL
DEL ..\Debug\Hebrew.lng
COPY /B Hebrew.lng /B ..\Debug\Hebrew.lng > NUL
DEL ..\Debug\Turkish.lng
COPY /B Turkish.lng /B ..\Debug\Turkish.lng > NUL
DEL ..\Debug\Spanish.lng
COPY /B Spanish.lng /B ..\Debug\Spanish.lng > NUL
DEL ..\Debug\Italian.lng
COPY /B Italian.lng /B ..\Debug\Italian.lng > NUL
DEL ..\Debug\Japanese.lng
COPY /B Japanese.lng /B ..\Debug\Japanese.lng > NUL
DEL ..\Debug\Danish.lng
COPY /B Danish.lng /B ..\Debug\Danish.lng > NUL
DEL ..\Debug\Russian.lng
COPY /B Russian.lng /B ..\Debug\Russian.lng > NUL
DEL ..\Debug\Macedonian.lng
COPY /B Macedonian.lng /B ..\Debug\Macedonian.lng > NUL
DEL ..\Debug\Hungarian.lng
COPY /B Hungarian.lng /B ..\Debug\Hungarian.lng > NUL
DEL ..\Debug\TraditionalChinese.lng
COPY /B TraditionalChinese.lng /B ..\Debug\TraditionalChinese.lng > NUL
DEL ..\Debug\Ukrainian.lng
COPY /B Ukrainian.lng /B ..\Debug\Ukrainian.lng > NUL
DEL ..\Debug\SimplifiedChinese.lng
COPY /B SimplifiedChinese.lng /B ..\Debug\SimplifiedChinese.lng > NUL
DEL ..\Debug\Greek.lng
COPY /B Greek.lng /B ..\Debug\Greek.lng > NUL

REM Copy latest language files into Release directory

DEL ..\Release\German.lng
COPY /B German.lng /B ..\Release\German.lng > NUL
DEL ..\Release\French.lng
COPY /B French.lng /B ..\Release\French.lng > NUL
DEL ..\Release\Dutch.lng
COPY /B Dutch.lng /B ..\Release\Dutch.lng > NUL
DEL ..\Release\Swedish.lng
COPY /B Swedish.lng /B ..\Release\Swedish.lng > NUL
DEL ..\Release\Korean.lng
COPY /B Korean.lng /B ..\Release\Korean.lng > NUL
DEL ..\Release\Czech.lng
COPY /B Czech.lng /B ..\Release\Czech.lng > NUL
DEL ..\Release\Estonian.lng
COPY /B Estonian.lng /B ..\Release\Estonian.lng > NUL
DEL ..\Release\Portuguese.lng
COPY /B Portuguese.lng /B ..\Release\Portuguese.lng > NUL
DEL ..\Release\Bulgarian.lng
COPY /B Bulgarian.lng /B ..\Release\Bulgarian.lng > NUL
DEL ..\Release\Slovak.lng
COPY /B Slovak.lng /B ..\Release\Slovak.lng > NUL
DEL ..\Release\Norwegian.lng
COPY /B Norwegian.lng /B ..\Release\Norwegian.lng > NUL
DEL ..\Release\Polish.lng
COPY /B Polish.lng /B ..\Release\Polish.lng > NUL
DEL ..\Release\Hebrew.lng
COPY /B Hebrew.lng /B ..\Release\Hebrew.lng > NUL
DEL ..\Release\Turkish.lng
COPY /B Turkish.lng /B ..\Release\Turkish.lng > NUL
DEL ..\Release\Spanish.lng
COPY /B Spanish.lng /B ..\Release\Spanish.lng > NUL
DEL ..\Release\Italian.lng
COPY /B Italian.lng /B ..\Release\Italian.lng > NUL
DEL ..\Release\Japanese.lng
COPY /B Japanese.lng /B ..\Release\Japanese.lng > NUL
DEL ..\Release\Danish.lng
COPY /B Danish.lng /B ..\Release\Danish.lng > NUL
DEL ..\Release\Russian.lng
COPY /B Russian.lng /B ..\Release\Russian.lng > NUL
DEL ..\Release\Macedonian.lng
COPY /B Macedonian.lng /B ..\Release\Macedonian.lng > NUL
DEL ..\Release\Hungarian.lng
COPY /B Hungarian.lng /B ..\Release\Hungarian.lng > NUL
DEL ..\Release\TraditionalChinese.lng
COPY /B TraditionalChinese.lng /B ..\Release\TraditionalChinese.lng > NUL
DEL ..\Release\Ukrainian.lng
COPY /B Ukrainian.lng /B ..\Release\Ukrainian.lng > NUL
DEL ..\Release\SimplifiedChinese.lng
COPY /B SimplifiedChinese.lng /B ..\Release\SimplifiedChinese.lng > NUL
DEL ..\Release\Greek.lng
COPY /B Greek.lng /B ..\Release\Greek.lng > NUL

REM Copy language reference file

COPY /B German.lng /B Util\LangRef.src > NUL

ECHO Have you updated the executable version information?
PAUSE
CLS