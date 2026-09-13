import type { Value } from "./value.d.ts";

/**
 * Supported Laskin AST node types.
 */
export type NodeType =
  "definition" | "literal" | "record" | "symbol" | "vector";

/**
 * Source code position of an AST node.
 */
export type Position = {
  path?: string;
  line: number;
  column: number;
};

/**
 * Representation of definition node in Laskin AST.
 */
export type DefinitionNode = {
  type: "definition";
  id: string;
  position?: Position;
};

/**
 * Representation of literal node in Laskin AST.
 */
export type LiteralNode = {
  type: "literal";
  value: Value;
  position?: Position;
};

/**
 * Representation of record literal node in Laskin AST.
 */
export type RecordNode = {
  type: "record";
  properties: Record<string, Node>;
  position?: Position;
};

/**
 * Representation of symbol node in Laskin AST.
 */
export type SymbolNode = {
  type: "symbol";
  id: string;
  position?: Position;
};

/**
 * Representation of vector literal node in Laskin AST.
 */
export type VectorNode = {
  type: "vector";
  elements: Node[];
  position?: Position;
};

/**
 * Representation of Laskin AST node.
 */
export type Node =
  DefinitionNode | LiteralNode | RecordNode | SymbolNode | VectorNode;
