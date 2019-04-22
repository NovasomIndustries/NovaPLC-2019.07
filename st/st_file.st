PROGRAM My_Program
  VAR
    button AT %IX0.0 : BOOL;
    lamp AT %QX0.0 : BOOL;
  END_VAR
  VAR
    T0 : TOF;
    NOT9_OUT : BOOL;
  END_VAR

  NOT9_OUT := NOT(button);
  T0(IN := NOT9_OUT, PT := T#2000ms);
  lamp := T0.Q;
END_PROGRAM


CONFIGURATION Config0

  RESOURCE Res0 ON PLC
    TASK TaskMain(INTERVAL := T#50ms,PRIORITY := 0);
    PROGRAM Inst0 WITH TaskMain : My_Program;
  END_RESOURCE
END_CONFIGURATION
