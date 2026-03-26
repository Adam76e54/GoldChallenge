final char comm_DELIMITER = ':'; // Commands will look like 'comm:payload'
final char comm_END = '\n';

final char comm_LEFT_IR = 'A';
final char comm_RIGHT_IR = 'B';

final char comm_KP = 'C';
final char comm_KI = 'D';
final char comm_KD = 'E';

final char comm_START = 'F';
final char comm_STOP = 'G';

final char comm_CURRENT_SPEED = 'H';
final char comm_SAVE_EEPROM = 'I';
  
final ArrayList<Character> comm_TIMES = new ArrayList<Character>(
  Arrays.asList('1', '2', '3', '4', '5')
);

final ArrayList<Character> comm_SPEEDS = new ArrayList<Character>(
  Arrays.asList('0', '6', '7', '8', '9')
);
