import { TodoList } from "./TodoList.js";

export default function Page() {
  return (
    <>
      <h1 className="text-2xl text-center w-full bg-green-500">To-do List</h1>
      <TodoList />
    </>
  );
}
