/**
 * @file workspace.h
 *
 * @date Apr 25, 2014
 * @author Craig Hesling
 */
#ifndef WORKSPACE_H_
#define WORKSPACE_H_

#define WORKSPACE_NAME_SIZE 10
#define WORKSPACE_SIZE 10

/// Indicates that a specified name has not been defined. For use in @ref workspace_get.
#define WORKSPACE_NOTSET ((void *)(long)(-1))

/// Indicates that the @ref workspace_set operation was successful.
#define WORKSPACE_OK   0
/// Indicates that the @ref workspace_set operation failed because the the workspace is full.
#define WORKSPACE_FULL 1
/// Indicates that the @ref workspace_set operation failed because the the name argument is invalid.
#define WORKSPACE_NAME 2

/**
 * Resets all workspace entries to unset.
 * @note Must be run before workspace use.
 */
void
workspace_init(void);

int
workspace_set(char *name, void *data);

void
workspace_unset(char *name);

void *
workspace_get(char *name);

#endif /* WORKSPACE_H_ */

/* vim: set ts=4 sw=4 expandtab: */
