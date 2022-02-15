#include <MFRC522.h>
#include <Vector.h>

const long BAUD = 115200;

const int ELEMENT_COUNT_MAX = 5;
typedef Vector<int> Elements;

void print_vec_int(Elements vector)
{
  int n = vector.size();
  for (int i = 0; i < n - 1; i++)
  {
    Serial.print(vector.at(i));
    Serial.print(",");
  }
  if (n > 0)
    Serial.print(vector.at(n - 1));
  Serial.println("");
}

void setup()
{
  Serial.begin(BAUD);
  sleep(1);

  int storage_array[ELEMENT_COUNT_MAX];
  Elements vector;
  vector.setStorage(storage_array);
  Serial.print("vector.max_size(): ");
  Serial.println(vector.max_size());
  Serial.print("vector.size(): ");
  Serial.println(vector.size());
  Serial.println("vector:");
  print_vec_int(vector);

  vector.push_back(10);
  vector.push_back(8);
  vector.push_back(7);
  Serial.print("vector.max_size(): ");
  Serial.println(vector.max_size());
  Serial.print("vector.size(): ");
  Serial.println(vector.size());
  Serial.println("vector:");
  print_vec_int(vector);
  vector.remove(0);
  Serial.println("vector.remove(0):");
  print_vec_int(vector);
  vector.remove(1);
  Serial.println("vector.remove(1):");
  print_vec_int(vector);

  int storage_array2[ELEMENT_COUNT_MAX];
  Elements vector2(storage_array2);
  vector2.push_back(1);
  vector2.push_back(2);
  vector2.push_back(4);
  vector2.pop_back();
  Serial.print("vector2.max_size(): ");
  Serial.println(vector2.max_size());
  Serial.print("vector2.size(): ");
  Serial.println(vector2.size());
  Serial.println("vector2:");
  print_vec_int(vector2);

  Serial.print("Print vector2 elements using iterators: ");
  for (int element : vector2)
  {
    Serial.print(element);
    Serial.print(" ");
  }
  Serial.println("");

  int storage_array3[ELEMENT_COUNT_MAX];
  storage_array3[0] = 3;
  storage_array3[1] = 5;
  Elements vector3(storage_array3);
  Serial.print("vector3.max_size(): ");
  Serial.println(vector3.max_size());
  Serial.print("vector3.size(): ");
  Serial.println(vector3.size());
  Serial.println("vector3:");
  print_vec_int(vector3);

  int storage_array4[ELEMENT_COUNT_MAX];
  storage_array4[0] = 3;
  storage_array4[1] = 5;
  Elements vector4(storage_array4, 2);
  Serial.print("vector4.max_size(): ");
  Serial.println(vector4.max_size());
  Serial.print("vector4.size(): ");
  Serial.println(vector4.size());
  Serial.println("vector4:");
  print_vec_int(vector4);

  int storage_array5[1];
  Elements vector5(storage_array5);
  Serial.print("vector5.max_size(): ");
  Serial.println(vector5.max_size());
  Serial.print("vector5.size(): ");
  Serial.println(vector5.size());
  Serial.println("vector5:");
  print_vec_int(vector5);

  int storage_array6[ELEMENT_COUNT_MAX];
  Elements vector6(storage_array6);
  vector6.assign(ELEMENT_COUNT_MAX - 1, 8);
  print_vec_int(vector5);
}

void loop() {}