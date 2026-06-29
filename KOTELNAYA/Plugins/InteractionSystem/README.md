# Interaction System Plugin

## Overview
The **Interaction System** is a plugin designed to handle player-to-world interactions (opening doors, picking up items, talking to NPCs). It manages object detection, UI prompts, and visual feedback (outlines), allowing you to focus solely on the gameplay logic.

The system consists of three main parts:
1.  **Interaction Component (Player):** The "eyes" of the player that scan for objects.
2.  **Interactable Target Component (Object):** The visual representation (UI Prompt & Outline).
3.  **Interactable Interface (Logic):** Defines what happens when the button is pressed.

---

## 1. Player Setup

To enable your character to interact with the world:

1.  Open your Character Blueprint (e.g., `BP_ThirdPersonCharacter`).
2.  Click **Add Component** and select **IHInteraction**.
3.  **Configuration (Details Panel):**
    * **Interaction Radius:** Set the detection range (Default: `200`).

### Input Setup (Action Mapping)
You need to bind input keys to trigger the component functions.

* **Interaction (e.g., "E" key):**
    * Call the `Perform Interaction` function on the component.
* **Cycling Targets (Optional - e.g., Mouse Wheel or Tab):**
    * If multiple objects are close to each other, you can switch focus between them.
    * Call `Cycle Interactable` passing `+1` (Next) or `-1` (Previous).

---

## 2. UI Widget Setup

The plugin requires a user widget to display the prompt.

1.  Create a new **Widget Blueprint** and name it `W_InteractionPrompt`.
2.  Go to **Graph** -> **Class Settings**.
3.  Set **Parent Class** to **IHInteractionPromptWidget**.
4.  **Designer Setup:**
    * **Action Text:** Add a `TextBlock`. In Details, check **Is Variable** and name it `ActionText`. The system will update this text automatically (e.g., "Open", "Pickup").
    * **Key Text (Optional):** Add another `TextBlock`. Check **Is Variable** and name it `KeyText`. This is used to display the input key (e.g., "E").

---

## 3. Creating an Interactable Object

To make an actor (e.g., a Chest or Key) interactable, you need to set up its Visuals (Prompt) and Logic (Interface).

### Step 1: Visual Setup (Components)
1.  Open the Actor Blueprint.
2.  **Add a Widget Component:**
    * This standard Unreal component will render your UI in the world.
    * **Widget Class:** Select your `W_InteractionPrompt`.
    * **Space:** Screen (recommended for prompts) or World.
    * **Draw at Desired Size:** True.
    * **Visibility:** Set to **Hidden** (the system will toggle it automatically).
    * Position it above your mesh.
3.  **Add the IHInteractableTarget Component:**
    * This component controls the widget and the outline.
    * **Prompt Widget:** The component tries to find the widget automatically. If it fails, assign your Widget Component here manually.
    * **Interaction Text:** Default text (e.g., "Pick Up").
    * **bEnableOutline:** Check this to enable Post-Process outlines on focus.

### Step 2: Logic Setup (Interface)
1.  Go to **Class Settings**.
2.  Under **Interfaces** -> **Implemented Interfaces**, add **IHInteractable**.
3.  In the **My Blueprint** panel -> **Interfaces**, you will see the interface functions.

---

## 4. Blueprint Implementation Examples

Here is how to implement the logic inside your Interactable Actor.

### Example A: Simple Pickup (The `Interact` Function)
Use this logic for items that are added to the inventory and destroyed.

1.  Right-click the `Interact` function in the Interfaces list -> **Implement Event**.
2.  **Get Instigator:** The event provides an `InstigatorPawn` (The Player).
3.  **Access Inventory:** Get the `IHInventoryComponent` from the Instigator.
4.  **Try Add Item:** Call `TryAddItem` using the Item ID from your Data Table.
5.  **Check Result:**
    * If `Remainder == 0` (Success): Call `DestroyActor`. Return `True`.
    * If `Remainder > 0` (Fail): Print "Inventory Full". Return `False`.

### Example B: Dynamic Prompts (The `GetInteractionActionText` Function)
Use this if you want the text to change based on the object's state (e.g., Open/Close Door).

1.  Double-click `GetInteractionActionText` in the Interfaces list.
2.  Create a boolean variable `bIsOpen`.
3.  Add a **Select** node connected to the Return Value.
    * If `bIsOpen` is True -> Return Text "Close".
    * If `bIsOpen` is False -> Return Text "Open".
4.  *Note: To force the widget to update immediately after interaction, call `RefreshWidgetText` on the `IHInteractableTarget` component.*

---

## 5. API Reference

### IHInteractable (Interface)
* `Interact(Instigator)`: Triggered by the player. Return `true` if successful.
* `GetInteractionActionText()`: Override to provide dynamic text.
* `OnFocusReceived()` / `OnFocusLost()`: Override for custom visual effects (e.g., playing a sound).

### IHInteractionComponent (Player)
* `PerformInteraction()`: Triggers the logic on the focused object.
* `CycleInteractable(Direction)`: Switches focus to the next/previous actor in range.

### IHInteractableTargetComponent (Object)
* `SetIsFocus(bool)`: Manually shows/hides the prompt.
* `RefreshWidgetText()`: Re-runs the `GetInteractionActionText` logic and updates the UI.