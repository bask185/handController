class Keypad {
    public:
        Keypad(unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
        void init(void);
        char readKeyPad();
       

    private:
        unsigned char keyPin[8];
    
        char keys[4][4] = {
            {'1','2','3','A'},
            {'4','5','6','B'},
            {'7','8','9','C'},
            {'*','0','#','D'} };

        
};
