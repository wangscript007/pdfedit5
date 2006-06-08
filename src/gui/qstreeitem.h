#ifndef __QSTREEITEM_H__
#define __QSTREEITEM_H__

#include <qobject.h>
#include "qscobject.h"

namespace gui {

class TreeItemAbstract;

/*=
 This type of object represents one item in treeview.
 One item in tree usually corespond to one object in PDF document
*/
/** \brief QObject wrapper around TreeItemAbstract (or its subclasses) */
class QSTreeItem : public QSCObject {
 Q_OBJECT
public slots:
 /*-
  Check if the tree item wrapper is valid,
  i.e. if corresponding tree item still exist in the tree view.
  Using most of invalid wrapper's functions will result in exception
  Return true if valid, false if not
 */
 bool valid();
 /*- Get object contained in this tree item */
 QSCObject* item();
 /*-
  Get object inside this tree item, as item() does, but with one exception:
  If the object is reference, return reference target instead of the reference
 */
 QSCObject* itemref();
 /*- Get type of object contained in this tree item (shortcut for .item().type()) */
 QString itemtype();
 /*- Return child of this tree item with given name, or NULL if this item have no child with such name */
 QSTreeItem* child(const QString &name);
 /*-
  Get names of all visible childs items of this treeitem 
  Childs that are not shown in tree (because of treeview settings, etc ...) are not returned.
  Childs that are "not yet known" (unopened references) are not returned too.
 */
 QStringList getChildNames();
 /*- Return parent of this tree item, or NULL if this item have no parent */
 QSTreeItem* parent();
 /*- Return name of this tree item */
 QString id();
 /*- Return caption of this tree item (sometimes can differ from name) */
 QString text();
 /*- Return path of this tree item (sequence of names from root item to this item, separated by slashes: "/" ) */
 QString path();
 /*- Explicitly reload contents of this item and its subtree from current state of PDF document */
 void reload();
 /*- Remove object in this tree item (and also any possible subitems) from document */
 void remove();
 // "undocumented" -> not to be used, except for debugging
 void reload_force();
public:
 QSTreeItem(TreeItemAbstract *item,BaseCore *_base);
 virtual ~QSTreeItem();
 TreeItemAbstract* get() const;
 void disable();
protected:
 QSTreeItem(const QString &className,TreeItemAbstract *item,BaseCore *_base);
protected:
 /** Object held in class*/
 TreeItemAbstract *obj;
};

} // namespace gui

#endif
