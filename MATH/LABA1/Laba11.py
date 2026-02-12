import tkinter as tk
from tkinter import messagebox
from itertools import combinations


print("Task 1")
A = set(map(int, input("Type first set: ").split()))
B = set(map(int, input("Type first set: ").split()))

print("The first set:", A)
print("The second set:", B)

if A == B:
    print("The sets are equal.")
elif A <= B:
    print("The first set is a subset of the second set.")
elif B <= A:
    print("The second set is a subset of the first set.")
else:
    print("The sets are different and none is a subset of the other.")


print("Task 2")

s = set(map(int, input("Type first set: ").split()))

subsets = [set()]
for element in s:
    new_subsets = []
    for subset in subsets:
        new_subset = subset | {element}
        new_subsets.append(new_subset)
    subsets += new_subsets

for subset in subsets:
    print(subset)


print("Task 3")

window = tk.Tk()
window.title("Візуалізація операцій над множинами")

canvas = tk.Canvas(window, width=800, height=600, bg="white")

canvas.pack()
canvas.create_oval(50 , 50 , 180 , 180 , fill = "light green")
canvas.create_oval(120 , 50 , 250 , 180 , fill = "light green")
label = tk.Label(window, text="A", bg="light green",font=("Arial", 20, ))
label.place(x=80, y=80)
label = tk.Label(window, text="B", bg="light green",font=("Arial", 20, ))
label.place(x=180, y=80)
label = tk.Label(window, text="A ∪ B", bg="white",font=("Arial", 25, ))
label.place(x=90, y=180)

canvas.create_oval(50 , 300 , 180 , 430 , fill = "light green")
canvas.create_oval(120 , 300 , 250 , 430 , fill = "light gray")
label = tk.Label(window, text="A", bg="light green",font=("Arial", 20, ))
label.place(x=80, y=320)
label = tk.Label(window, text="B", bg="light gray",font=("Arial", 20, ))
label.place(x=180, y=320)
label = tk.Label(window, text="A - B", bg="white",font=("Arial", 25, ))
label.place(x=90, y=430)

canvas.create_oval(500 , 50 , 630 , 180 , fill = "light gray")
canvas.create_oval(570 , 50 , 700 , 180 , fill = "light gray")
canvas.create_oval(570 , 60 , 625 , 170 , fill = "light green", )
label = tk.Label(window, text="A", bg="light gray",font=("Arial", 20, ))
label.place(x=530, y=80)
label = tk.Label(window, text="B", bg="light gray",font=("Arial", 20, ))
label.place(x=630, y=80)
label = tk.Label(window, text="A ∩ B", bg="white",font=("Arial", 25, ))
label.place(x=550, y=180)

canvas.create_oval(500 , 300 , 630 , 430 , fill = "light green", )
canvas.create_oval(570 , 300 , 700 , 430 , fill = "light green" ,)
canvas.create_oval(570 , 310 , 625 , 420 , fill = "light gray", )
label = tk.Label(window, text="A", bg="light green",font=("Arial", 20, ))
label.place(x=530, y=320)
label = tk.Label(window, text="B", bg="light green",font=("Arial", 20, ))
label.place(x=630, y=320)
label = tk.Label(window, text="A ⊕ B", bg="white",font=("Arial", 25, ))
label.place(x=550, y=430)

window.mainloop()