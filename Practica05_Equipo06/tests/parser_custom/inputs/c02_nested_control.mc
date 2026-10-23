int x = 10;
bool activo = true;

while (x > 0 && activo) {
    if (x == 5) {
        print(x);
    } else {
        if (x < 3)
            print(0);
        else
            print(x);
    }

    x = x - 1;
}
