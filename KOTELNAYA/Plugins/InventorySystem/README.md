# Inventory System Plugin

## Overview
The **Inventory System** is a lightweight, C++ based solution for managing item storage in Unreal Engine. It separates **Static Data** (definitions like Name, Icon, Mesh) from **Dynamic Data** (what is currently inside a specific slot).

**Key Features:**
* **Centralized Database:** All items are defined in a single Data Table.
* **Smart Stacking:** Automatically merges items into existing stacks before occupying new slots.
* **Drag & Drop Logic:** Built-in `TransferItem` function handles swapping slots and merging stacks.
* **Event-Driven:** Provides delegates for UI updates to avoid checking state in Tick.

---

## 1. Initial Setup (Required)

Before using the inventory component, you must configure the global item database.

### Step 1: Create the Item Data Table
1. In the **Content Browser**, right-click and select **Miscellaneous** -> **Data Table**.
2. Pick the Row Structure: **IHItemDefinition**.
3. Name the asset (e.g., `DT_GameItems`).

### Step 2: Link to Project Settings
The plugin requires a reference to this table to load item properties (like Max Stack Size) automatically.
1. Go to **Edit** -> **Project Settings**.
2. Navigate to the **Game** section -> **Inventory System**.
3. In the **Item Data Table** property, select your created table (`DT_GameItems`).

> **Warning:** If you skip Step 2, the inventory will not be able to identify items, and `TryAddItem` will always fail.

---

## 2. Defining Items

To add items to your game, simply add rows to your `DT_GameItems` Data Table.

* **Row Name:** The unique **Item ID** used in Blueprints (e.g., `Key_Red`, `Potion_Small`).
* **Name:** The display name shown in the UI.
* **Description:** Text for tooltips or inspection windows.
* **Icon:** The 2D Texture for the UI slot.
* **Mesh:** The Static Mesh used when the item is dropped or inspected in 3D.
* **Max Stack Size:**
    * Set to `1` for unique items (Weapons, Quest Items).
    * Set to higher values (e.g., `10`, `99`) for consumables or resources.

---

## 3. Using the Component

Add the `IHInventoryComponent` to any Actor that needs to store items (Player Character, Chests, NPCs).

1. Open your Actor Blueprint.
2. Click **Add Component** and select **IHInventory**.
3. In the **Details Panel**, set the **Max Slots** (e.g., `10` for a Player, `20` for a Chest).

---

## 4. Blueprint API Reference

### `TryAddItem`
Attempts to add an item to the inventory. It prioritizes filling existing stacks first.

* **Inputs:**
    * `ItemID` (Name): The Row Name from your Data Table.
    * `Amount` (Integer): Quantity to add.
* **Returns:** `Integer` (Remainder).
    * **0:** Success. All items were added.
    * **> 0:** Inventory Full. The returned value is the amount that **could not** be added.
* **Example:** You try to pick up 5 Apples. If the function returns `2`, it means 3 were added, but 2 are still left on the ground.

### `RemoveItem`
Removes a specific quantity of an item.

* **Inputs:** `ItemID`, `AmountToRemove`.
* **Returns:** `Boolean` (True if the items were successfully found and removed).

### `TransferItem` (Drag & Drop)
Moves an item from one slot to another. This works within a single inventory or between two different components (e.g., Player to Chest).

* **Logic:**
    1. **Merge:** If the Source and Target items are the same, it tries to combine the stacks.
    2. **Swap:** If the items are different (or the target stack is full), it swaps the two slots.
* **Inputs:**
    * `FromIndex`: Slot index of the item being moved.
    * `TargetComponent`: The component receiving the item.
    * `ToIndex`: The slot index where the item is dropped.

### `HasItem`
Checks if the inventory contains at least a specific amount of an item.
* **Returns:** `True` if `Total Quantity >= Quantity Needed`.

### `GetItemDefinition`
A static library function to get item data (Icon, Mesh, etc.) without needing a component reference.
* **Location:** `IHInventoryFunctionLibrary`.

---

## 5. UI Integration

Do **not** use `Event Tick` to update your UI. Bind your widgets to these Event Dispatchers on the Inventory Component.

| Event Name | Trigger | Recommended Action |
| :--- | :--- | :--- |
| **OnInventoryUpdated** | Any change (Add, Remove, Move) | Redraw the inventory grid using `GetSlots`. |
| **OnItemAdded** | Items successfully added | Show a popup: *"Received [Item] x [Amount]"*. |
| **OnItemAddFailed** | Inventory full during add attempt | Show a warning: *"Inventory Full!"*. |

### Rendering Slots
To display items in your widget:
1. Call `GetSlots` to get the array of `FIHInventorySlot`.
2. Loop through the array.
3. If `Slot.IsValid()` is **True**:
    * Get the ID via `Slot.GetItemID()`.
    * Use `GetItemDefinition(ID)` to find the **Icon**.
    * Display `Slot.Quantity`.