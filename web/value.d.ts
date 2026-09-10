import type { Node } from "./ast.d.ts";

/**
 * Supported Laskin value types.
 */
export type ValueType =
  | "boolean"
  | "date"
  | "month"
  | "number"
  | "quote"
  | "record"
  | "string"
  | "time"
  | "vector"
  | "weekday";

/**
 * Enumeration of month names in Laskin.
 */
export type Month =
  | "january"
  | "february"
  | "march"
  | "april"
  | "may"
  | "june"
  | "july"
  | "august"
  | "september"
  | "october"
  | "november"
  | "december";

/**
 * Enumeration of weekday names in Laskin.
 */
export type Weekday =
  | "sunday"
  | "monday"
  | "tuesday"
  | "wednesday"
  | "thursday"
  | "friday"
  | "saturday";

/**
 * Representation of boolean value in Laskin.
 */
export type BooleanValue = {
  type: "boolean";
  value: boolean;
};

/**
 * Representation of numeric value in Laskin.
 */
export type NumberValue = {
  type: "number";
  value: string;
  unit?: string;
};

/**
 * Representation of string value in Laskin.
 */
export type StringValue = {
  type: "string";
  value: string;
};

/**
 * Representation of month value in Laskin.
 */
export type MonthValue = {
  type: "month";
  value: Month;
};

/**
 * Representation of weekday value in Laskin.
 */
export type WeekdayValue = {
  type: "weekday";
  value: Weekday;
};

/**
 * Representation of date value in Laskin.
 */
export type DateValue = {
  type: "date";
  year: number;
  month: Month;
  day: number;
};

/**
 * Representation of time value in Laskin.
 */
export type TimeValue = {
  type: "time";
  hour: number;
  minute: number;
  second: number;
};

/**
 * Representation of vector value in Laskin.
 */
export type VectorValue = {
  type: "vector";
  elements: Value[];
};

/**
 * Representation of record value in Laskin.
 */
export type RecordValue = {
  type: "record";
  properties: Record<string, Value>;
};

/**
 * Representation of quote in Laskin.
 */
export type QuoteValue = {
  type: "quote";
  /** AST nodes contained in the quote. */
  nodes: Node[];
};

/**
 * Representation of Laskin value.
 */
export type Value =
  | BooleanValue
  | DateValue
  | MonthValue
  | NumberValue
  | QuoteValue
  | RecordValue
  | StringValue
  | TimeValue
  | VectorValue
  | WeekdayValue;
